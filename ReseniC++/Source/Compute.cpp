/*
  ==============================================================================

	Compute.cpp
	Created: 6 Apr 2018 2:55:14am
	Author:  Matouš

  ==============================================================================
*/

#include "Compute.h"


using namespace std;

Compute::Compute()
{
}
Compute::~Compute()
{}

#pragma region Public
void Compute::IRSweep(AudioBuffer<float> &inputSignalPointer, AudioBuffer<float> &recordedDataPointer, AudioBuffer<float> &irDataPointer)
{
	auto *inputSignalReader = inputSignalPointer.getReadPointer(0);
	
	int inpSize = inputSignalPointer.getNumSamples();
	int outpSize = recordedDataPointer.getNumSamples();
	int irDataSize = irDataPointer.getNumSamples();
	int nfft = log2(nextPowerOfTwo(inputSignalPointer.getNumSamples()));		//the n of n^2
	int nfftp = pow(2, nfft);							//the n^2

	FFT res(nfft);		//takes argument - nfft, but creates size of 2^nfft


	for (int channel = 0; channel < recordedDataPointer.getNumChannels(); ++channel)
	{
		ScopedPointer<Complex<float>> In, Out, ConjOut, Sbb, Sab, H2, tempSbbMul,tempSabMul;	//they all need to be deleted
		auto *recordedDataChannelReader = recordedDataPointer.getReadPointer(channel);
		auto *irDataChannelWriter = irDataPointer.getWritePointer(channel);
		

		In = FFTfx(inputSignalReader, inpSize, nfft) ;		//obraz vstupu		
		Out = FFTfx(recordedDataChannelReader, outpSize, nfft) ;		//obraz výstupu
		
		//podil = CpDiv(Out, In, nfftp);
		
		ConjOut = ConjugateArray(Out, nfftp);

		tempSbbMul = CpMul(Out, ConjOut, nfftp);
		Sbb =  CpDiv(tempSbbMul, pow(nfftp, 2.0), nfftp) ;	//takes CpDiv with non reference input, CpMul creates result in memory and passes it right to the CpDiv,
		tempSabMul = CpMul(In, ConjOut, nfftp);
		Sab = CpDiv(tempSabMul, pow(nfftp, 2.0), nfftp) ;

		H2 = CpDivR(Sbb, Sab, nfftp) ;

		res.perform(H2, Out, true);	// just using Out as output, so i dont have to initialize a new variable
		
		//write only real data data and also normalized
		CpToMagnitude(Out, irDataChannelWriter , irDataSize);
	}


}
void Compute::IRMLS(AudioBuffer<float> &irDataPointer, AudioBuffer<float> &recordedDataPointer, int Order)
{
	int lngt = pow(2, Order) - 1;
	float div = pow(2, Order);
	
	for (int channel = 0; channel < recordedDataPointer.getNumChannels(); ++channel)
	{
		auto *recordedDataChannelReader = recordedDataPointer.getReadPointer(channel);
		auto *irDataChannelWriter = irDataPointer.getWritePointer(channel);

		ScopedPointer< vector<float>> signal;
		signal = new vector<float>(lngt);											//length only of lngt

		for (int i = 0; i < lngt; ++i)
		{
			signal->operator[](i) = recordedDataChannelReader[i];
		}

		const int *P1, *P2;// = importTags(1, Order);		//import tags1
		switch (Order)
		{
		case 3: P1 = MLSTags::P1_3;
			P2 = MLSTags::P2_3;
			break;
		case 4: P1 = MLSTags::P1_4;
			P2 = MLSTags::P2_4;
			break;
		case 5: P1 = MLSTags::P1_5;
			P2 = MLSTags::P2_5;
			break;
		case 6: P1 = MLSTags::P1_6;
			P2 = MLSTags::P2_6;
			break;
		case 7: P1 = MLSTags::P1_7;
			P2 = MLSTags::P2_7;
			break;
		case 8: P1 = MLSTags::P1_8;
			P2 = MLSTags::P2_8;
			break;
		case 9: P1 = MLSTags::P1_9;
			P2 = MLSTags::P2_9;
			break;
		case 10: P1 = MLSTags::P1_10;
			P2 = MLSTags::P2_10;
			break;
		case 11: P1 = MLSTags::P1_11;
			P2 = MLSTags::P2_11;
			break;
		case 12: P1 = MLSTags::P1_12;
			P2 = MLSTags::P2_12;
			break;
		case 13: P1 = MLSTags::P1_13;
			P2 = MLSTags::P2_13;
			break;
		case 14: P1 = MLSTags::P1_14;
			P2 = MLSTags::P2_14;
			break;
		case 15: P1 = MLSTags::P1_15;
			P2 = MLSTags::P2_15;
			break;
		case 16: P1 = MLSTags::P1_16;
			P2 = MLSTags::P2_16;
			break;
		case 17: P1 = MLSTags::P1_17;
			P2 = MLSTags::P2_17;
			break;
		case 18: P1 = MLSTags::P1_18;
			P2 = MLSTags::P2_18;
			break;
		}


		ScopedPointer< vector<float>> temp1;
		temp1 = new vector<float>(lngt + 1);				//vector which stores signal after first permutation

		//first permutation
		for (int i = 0; i < lngt; ++i)
		{
			temp1->operator[](i + 1) = signal->operator[](P1[i] - 1);
		}

		//signal.clear();
		//vector<float>(signal).swap(signal);		//free some memory?

		ScopedPointer< vector<float>> HY1;
		HY1 = new vector<float>(HadamardTransform(temp1));

		//temp1.clear();

		ScopedPointer<vector<float>> temp2;
		temp2 = new vector<float>(lngt);

		//second permutation
		for (int i = 0; i < lngt; ++i)
		{
			temp2->operator[](i) = HY1->operator[](P2[i]);
		}

		//vector<float> IR(lngt);

		//i just need to write this to passed reference of irData
		for (int i = 0; i < lngt; ++i)
		{
			irDataChannelWriter[i] = (1 / div) * temp2->operator[](i);;
		}

	}
}
void Compute::LundebysMethod(AudioBuffer<float> &irDataPointer, AudioBuffer<float> &lundebyDataPointer, Array<LundebyReturn> &lundebyAdditionalData, int fs)
{
	LundebyReturn result;
	double dBStartMulti = 0.1;      //od kdy dělám výpočet regrese      
	double dBUpMulti = 0.25;        //do kdy dělám výpočet regrese
	double dBDownMulti = 0.2;       //o kolik dolů budu posouvat crosspoint

	double dBUp;                    //DO kdy se bude počítat decay
	double dBStart;                 //OD kdy se bude počítat decay
	double dBDown;                  //posun pro crosspoint (kde se protne decay přímka s touto hodnotou, bude nový crosspoint)

	double noise;

	int predelay;                   //sample, kde je nejvyšší hodnota
	int tDecayStart = 0;            //sample, kterej je první nižší o dbDStart než maximální hodnota
	int t10Stop;					//smaple, kterej je první nižší o 10 db než dBStart
	int t20Stop;					//smaple, kterej je první nižší o 10 db než dBStart
	int t30Stop;					//smaple, kterej je první nižší o 10 db než dBStart
	int tDecayStop;					//sample, kterej je první nižší než je noise + dbUp;
	int ms = (int)( irDataPointer.getNumSamples()/(fs*0.4) +2);                    //defaultní počet milisekund k integraci 5 ms (not anymore) určuje se dynamicky zhruba od 2 do 8 ms
	int newCrosspoint = 0;          //průsečík přímky regrese a noise floor
	int newCrosspointTemp = 0;      //dočasná hodnota
	float maxvalue;
	DecayParameters regression, regression10, regression20, regression30;

	for (int channel = 0; channel < irDataPointer.getNumChannels(); ++channel)
	{
		auto * irChannelReader = irDataPointer.getReadPointer(channel);
		auto * lundebyChanneWriter = lundebyDataPointer.getWritePointer(channel);

		vector<float> IRs = SquaredArr(irChannelReader, irDataPointer.getNumSamples());
		// (1)
		vector<float> env = MovingAverage(IRs, fs, ms, false);
		env = Log10N(env);              //ošetřený i proti -Infinity

		double dynrange = DynRange(env);

		//podle dynrange nastavím parametry
		dBUp = dBUpMulti * dynrange;         //DO kdy se bude počítat decay
		dBStart = -dBStartMulti * dynrange;  //OD kdy se bude počítat decay
		dBDown = dBDownMulti * dynrange;     //posun pro crosspoint (kde se protne decay přímka s touto hodnotou, bude nový crosspoint)

		// (2)
		//začnu třeba na 50% délky, tam je pravděpodobnost, že už jsem v IR na hodnotě hluku pozadí, i taková, že jsem pořád v poklesu
		int t1 = roundToInt(IRs.size() / 2);

		noise = Mean(env, t1, env.size() - 1);

		// (3)
		vector<float>::iterator predelayIt;
		predelayIt = max_element(env.begin(), env.end());
		predelay = distance(env.begin(), predelayIt);//   Array.IndexOf(env, env.Max());       //sampl, kde je nejvyšší hodnota
		tDecayStop = FirstLessThan(env, dBUp + noise, predelay, env.size() - 1);  //sampl, který je první nižší než hladina dBUp nad úrovní šumu


		vector<float> xdata(env.size(), 0.0);       //array ktery reprezentuje x souradnice decaye
		for (int i = 0; i < xdata.size(); i++)
			xdata[i] = (float)(i);


		//vypočítám přímku regrese poklesu pouze v intervalu od predelaye po t30Stop, tam je pokles nejrovnoměrnější

		regression = Compute::LinLSSolver(xdata, env, predelay, tDecayStop);

		// (5)
		double deltax = -dynrange / regression.Slope;

		int newpocet = 40;                                  //ten interval rozdělím na 40 intervalů, to mi dá docela dobrý rozlišení při jakýmkoli dynrange
		int newinterval = (int)((deltax / (double)newpocet) + 0.5);    //budu to dělat podle t20

		// (6)
		if (newinterval < 0)	//if regression fails
		{
			env = MovingAverage(IRs, fs, ms, false);     //tentokrát true - zadávám přesnej počet samplů k integraci
		}
		else
		{
		env = MovingAverage(IRs, fs, newinterval, true);     //tentokrát true - zadávám přesnej počet samplů k integraci
		}
		env = Log10N(env);

		predelayIt = max_element(env.begin(), env.end());		//musím změnit i iterator - je to jako ukazatel, takže jestliže změním env, tak ten iterator už neukazuje tam kam předtím
		predelay = distance(env.begin(), predelayIt);   //znovu najdu nejvyšší hodnotu

		dynrange = DynRange(env, predelay);     //po nové MovingAverage se nejvyšší a nejnižší hodnoty změní
		dBUp = dBUpMulti * dynrange;         //DO kdy se bude počítat decay
		dBStart = -dBStartMulti * dynrange;  //OD kdy se bude počítat decay
		dBDown = dBDownMulti * dynrange;     //posun pro crosspoint (kde se protne decay přímka s touto hodnotou, bude nový crosspoint)



		// --------

		for (int cycles = 0; cycles < 3; cycles++)        //počet iteračních cyklů, 3 je většinou až až k dobrýmu určení noise level a správnýmu poklesu
		{
			// (4)
			/*
			Mám 2 crosspointy. Pokud bych už nenašel hodnotu, která je od předchozího noise floor nižší o dBDown,
			tak pouze novej crosspoint pouze posunu o polovinu blíž ke konci
			*/

			newCrosspoint = FirstLessThan(regression.Slope, regression.Intercept, noise - dBDown);        //y1 jsou funkční hodnoty až od x = predelay, tak to musím posunout, abych měl pozici novýho crosspointu v IRs

			if (newCrosspoint < 0 || newCrosspoint>irDataPointer.getNumSamples())
				newCrosspoint = newCrosspointTemp + (IRs.size() - newCrosspointTemp) / 2;
			else
				newCrosspointTemp = newCrosspoint;

			noise = Mean(env, newCrosspoint, env.size() - 1);   //noise floor budu počítat od novýho crosspointu do konce 

			// (7)
			tDecayStart = LastGreaterThan(env, dBStart) + 1;		//najdu index, kterej je poslední větší než dbStart - může tam bejt nějakej velkej propad za první early reflection, tak proto nepoužívám FirstLessThan
			tDecayStop = FirstLessThan(env, dBUp + noise, tDecayStart, env.size() - 1);

			regression = Compute::LinLSSolver(xdata, env, tDecayStart, tDecayStop);
		}
		t10Stop = FirstLessThan(env, dBStart - 10.0, tDecayStart, tDecayStop);	//sampl, který je první nižší než dBStart - 10 
		t20Stop = FirstLessThan(env, dBStart - 20.0, tDecayStart, tDecayStop);		//sampl, který je první nižší než dBStart - 20
		t30Stop = FirstLessThan(env, dBStart - 30.0, tDecayStart, tDecayStop);		//sampl, který je první nižší než dBStart - 30
		
		regression10 = Compute::LinLSSolver(xdata, env, tDecayStart, t10Stop);
		regression20 = Compute::LinLSSolver(xdata, env, tDecayStart, t20Stop);
		regression30 = Compute::LinLSSolver(xdata, env, tDecayStart, t30Stop);
		double deltax10 = -60.0 / regression10.Slope;       //vypočítám, kolik vzorků uběhne, než průběh poklesne o hodnotu dynrange
		double deltax20 = -60.0 / regression20.Slope;
		double deltax30 = -60.0 / regression30.Slope;

		deltax = -60.0 / regression.Slope;

		
		//result.Envelope = env;
		for (int i = 0; i < env.size(); ++i)
		{
			lundebyChanneWriter[i] = env[i];
		}

		result.DecayStartpoint = tDecayStart;
		result.DecPar10 = regression10;
		result.DecPar20 = regression20;
		result.DecPar30 = regression30;
		result.NoiseLvl = noise;
		result.Predelay = predelay;
		result.Corsspoint = newCrosspoint;
		result.T10 = deltax10 / (float)fs;
		result.T20 = deltax20 / (float)fs;
		result.T30 = deltax30 / (float)fs;
		result.T60 = deltax / (float)fs;
		result.IntegrInterval = newinterval;

		lundebyAdditionalData.set(channel, result);
	}

}
void Compute::SchroederInt(AudioBuffer<float> &irDataPointer, AudioBuffer<float> &schroederDataPointer, Array<SchroederReturn> &schroederAdditionalData, Array<LundebyReturn> &lundebyAdditionalData, const float fs)
{
	SchroederReturn result;

	for (int channel = 0; channel < irDataPointer.getNumChannels(); ++channel)
	{
		auto * irChannelReader = irDataPointer.getReadPointer(channel);
		auto * schroederChanneWriter = schroederDataPointer.getWritePointer(channel);
		int noiseCrosspoint = lundebyAdditionalData[channel].Corsspoint;
		int predelay = lundebyAdditionalData[channel].Predelay;

		vector<float> IRs = SquaredArr(irChannelReader, noiseCrosspoint);		//only to the crosspoint
		vector<float> summed = ReversedSum(IRs, predelay);
		vector<float> env = Log10N(summed);

		int lngt = (int)((env.size()-predelay)*0.75);

		double dynrange = DynRange(env, predelay, predelay + lngt);               //dynrange od začátku (nejvyšší hodnota) po 0.75*délka - konec padá dolů, na nic
		double dBStartMulti = 0.1;                              //zase 
		double dBEndMulti = 0.75;

		double dBStart = -dBStartMulti * dynrange;              // budu odečítat sklon přímky od 0.1 dynrange po 0.75 dynrange (záporný to je protože max je na začátku a je to 0)
		double dBEnd = -dBEndMulti * dynrange;

		int tDecayStart = FirstLessThan(env, dBStart);
		int tDecayStop = FirstLessThan(env, dBEnd);
		int t10Stop = FirstLessThan(env, dBStart -10.0);
		int t20Stop = FirstLessThan(env, dBStart - 20.0);
		int t30Stop = FirstLessThan(env, dBStart - 30.0);

		vector<float> xdata(env.size(), 0.0);       //array ktery reprezentuje x souradnice decaye
		for (int i = 0; i < xdata.size(); i++)
			xdata[i] = (float)(i);


		DecayParameters regression = LinLSSolver(xdata, env, tDecayStart, tDecayStop);
		DecayParameters regression10 = LinLSSolver(xdata, env, tDecayStart, t10Stop);
		DecayParameters regression20 = LinLSSolver(xdata, env, tDecayStart, t20Stop);
		DecayParameters regression30 = LinLSSolver(xdata, env, tDecayStart, t30Stop);

		float deltax = -60.0 / regression.Slope;
		float deltax10 = -60.0 / regression10.Slope;
		float deltax20 = -60.0 / regression20.Slope;
		float deltax30 = -60.0 / regression30.Slope;

		for (int i = 0; i < env.size() ; ++i)
		{
			schroederChanneWriter[i] = env[i];
		}
		for (int i = env.size(); i < irDataPointer.getNumSamples(); ++i)
		{
			schroederChanneWriter[i] = env[noiseCrosspoint-1];	//repeat the last one (the least) till end

		}


		result.DecPar10 = regression10;
		result.DecPar20 = regression20;
		result.DecPar30 = regression30;
		result.T10 = deltax10 / float(fs);;
		result.T20 = deltax20 / float(fs);
		result.T30 = deltax30 / float(fs);
		result.T60 = deltax / float(fs);
		schroederAdditionalData.set(channel, result);
	}
}
void Compute::FilterIR(AudioBuffer<float> &irDataPointer, const float fs, const float cutOffFrequency)
{
	ScopedPointer<IIRFilter> highpass;
	ScopedPointer<IIRCoefficients> coeffs;
	highpass = new IIRFilter();
	coeffs = new IIRCoefficients(IIRCoefficients::makeHighPass(fs, cutOffFrequency, 1.0));
	highpass->setCoefficients(*coeffs.get());
	for (int channel = 0; channel < irDataPointer.getNumChannels(); ++channel)
	{
		auto * writer = irDataPointer.getWritePointer(channel);
		highpass->processSamples(writer, irDataPointer.getNumSamples());
		highpass->reset();
	}
}
void Compute::NormalizeBuffer(AudioBuffer<float> &signalPointer)
{
	float maxBufferValue;
	for (int channel = 0; channel < signalPointer.getNumChannels(); ++channel)
	{
		auto range = signalPointer.findMinMax(channel, 0, signalPointer.getNumSamples());
		maxBufferValue = range.getEnd();
		if (abs(range.getStart()) > range.getEnd())
		{
			maxBufferValue = abs(range.getStart());
		}
	}

	for (int channel = 0; channel < signalPointer.getNumChannels(); ++channel)
	{
		signalPointer.applyGain(channel, 0, signalPointer.getNumSamples(), 1.0 / maxBufferValue);
	}
}

#pragma endregion


#pragma region Private

//IRMLS 
/**
Performs Hadamard transform of input signal
P:
@param input - vector<float>, signal to be permutated (should be n^2 length but accepts n^2-1 too)
@return vector<float>, new vector of permutated input signal (n^2 length)
*/
vector<float> Compute::HadamardTransform(ScopedPointer<vector<float>> input) //i'm passing it here by value, but i want to change it here, but not affect it outside, so i would make a copy anyway
{
	int lngt = input->size();                //length of input
	int n = (int)(log2(lngt));     //order of mls



	if ((int)pow(2, n) != lngt)          //checks if signal is zero pre-padded, if not, fixes it (butterfly algorithm works only for 2^n signals
	{
		lngt++;
		input->insert(input->begin(), 0.0);
	}

	vector<float> temp(lngt, 0.0);         //zeros by default

	for (int j = 1; j <= n; j++)        //butterfly algorithm
	{
		for (int i = 0; i < lngt - j; i += (int)pow(2, j))
		{
			for (int k = 1; k <= (int)pow(2, j - 1); k++)
			{
				temp[i + (k - 1)] = input->operator[](i + (k - 1)) + input->operator[](i + (int)pow(2, j - 1) + (k - 1));
				temp[i + (int)pow(2, j - 1) + (k - 1)] = input->operator[](i + (k - 1)) - input->operator[](i + (int)pow(2, j - 1) + (k - 1));
			}
		}
		input->operator= (temp);
	}
	return temp;


}
/**
Imports tags to vector from a binary file, location is fixed by now.
P:
@param tagNumber int, 1 or 2, there are 2 types of permutation matrices
@param order int, order of mls signal
@return vector<float>, vector of tag numbers
*/
vector<int> Compute::importTags(int tagNumber, int order)
{
	int numbers = (int)pow(2, order) - 1;
	vector<int> tags(numbers);

	stringstream ss;
	ss << "tags\\P" << tagNumber << "(" << order << ").bin";		//takhle budu číst správnej soubor

	const string path = ss.str();
	fstream file(path, ios::binary | ios::in);
	char buffer[4];

	for (int i = 0; i < numbers; i++)
	{
		file.read(buffer, sizeof(buffer));	//přečte se vždycky 4 chary - char - 8bit -> 32bit int, každej char reprezentuje 8 bitů od lsb skupinu po msb skupinu
		tags[i] = CharToInt(buffer);
	}
	file.close();
	return tags;
}
/**
Converts passed char[4] to int value. Serves for converting from hex to dec.
P:
@param chars char*, char[4] that contains one int number. Conversion from char is performed to hex and than properly to dec
@return vector<int>, vector of tags numbers
*/
int Compute::CharToInt(char* chars)
{
	int result = 0;
	int temp;
	int lsb;			//char má velikost 256 max, takže v hex FF, take má jen 2 hex bity msb lsb 
	int msb;
	for (int i = 0; i < 4; i++)
	{
		if (chars[i] < 0)
		{
			temp = 256 + (int)chars[i];		//když bude char zápornej, tak přehodím do kladnýho
		}
		else
		{
			temp = chars[i];
		}
		if (temp == 0)
			continue;						//když bude 0 tak se tím nebudu zabývat
		lsb = temp % 16;					//zbytek po dělení je lsb
		msb = (temp - lsb) / 16;			//když odečtu zbytek, tak to můžu dělit beze zbytku a dostanu druhou hodnotu hex - msb
		result += lsb * pow(16, 2 * i);		// převedu z hex na dec
		result += msb * pow(16, 2 * i + 1);	
	}
	return result;
}

//IRSweep
/**
Performs FFT on passed signal.
P:
@param &input vector<float>, signal to be FFT transformed
@param nfft int, order of FFT - needs to be n^2.
@return juce::dsp::Complex<float>, complex Fourier transformed input signal
*/
Complex<float>* Compute::FFTfx(const float* recordedDataChannelReadPointer, const int samples, const int nfft)
{
	int nfftp = pow(2, nfft);
	ScopedPointer<Complex<float>> fft;	//needs to be deleted
	Complex<float> *fxoutput;
	fft = new Complex<float>[nfftp];
	fxoutput = new Complex<float>[nfftp];

	for (int i = 0; i < samples; i++)
	{
		fft[i] = Complex<float>(recordedDataChannelReadPointer[i], 0.0);
	}
	for (int i = samples; i < nfftp; i++)
	{
		fft[i] = Complex<float>(0.0, 0.0);
	}
	ScopedPointer< FFT> In;
	In = new FFT(nfft) ;		//this takes nfft not nfftp
	In->perform(fft, fxoutput, false);	//prostě obraz vstupu, ale žere to jen komplexní signály tohoto specifickýho typu, tak jsem to zabalil
	//delete[] fft;
	return fxoutput;

}
/**
Returns conjugate complex signal to input.
P:
@param &input juce::dsp::Complex<float>, input signal to b
@param inpSize int, length of input signal
@return juce::dsp::Complex<float>, conjugate to the input signal
*/
Complex<float>* Compute::ConjugateArray(ScopedPointer<Complex<float>> &input, const int inpSize) //Returns conjugate array of comlex input.
{
	dsp::Complex<float> *output;
	output = new Complex<float>[inpSize];

	for (int i = 0; i < inpSize; i++)
	{
		output[i] = Complex<float>(input [i] .real(), -input[i].imag());		//conjugate - má prostě jen zápornou fázi
	}
	return output;
}
/**
Performs complex multiplication
P:
@param &left juce::dsp::Complex<float>, left operand
@param &right juce::dsp::Complex<float>, right operand
@param inpSize int, length of input signal
@return juce::dsp::Complex<float>
*/
Complex<float>* Compute::CpMul(ScopedPointer< Complex<float>> &left, ScopedPointer< Complex<float>> &right, const int inpSize)
{
	Complex<float>* output;
	output = new Complex<float>[inpSize];
	for (int i = 0; i < inpSize; i++)
	{
		output[i] = left[i] * right[i];
	}
	return output;
}
/**
Performs complex division, parametrs are passed by reference
P:
@param &nom juce::dsp::Complex<float>, nominator
@param &denom juce::dsp::Complex<float>, denominator
@param inpSize int, length of input signals (should be equal, not handled)
@return juce::dsp::Complex<float>
*/
Complex<float>* Compute::CpDivR(ScopedPointer< Complex<float>> &nom, ScopedPointer< Complex<float>> &denom, const int inpSize)
{
	Complex<float>* output;
	output = new Complex<float>[inpSize];
	for (int i = 0; i < inpSize; i++)
	{
		output[i] = nom[i] / denom[i];
	}
	return output;
}
/**
Performs complex division, parametrs are passed by value
P:
@param nom juce::dsp::Complex<float>, nominator
@param denom juce::dsp::Complex<float>, denominator
@param inpSize int, length of input signals (should be equal, not handled)
@return juce::dsp::Complex<float>
*/
Complex<float>* Compute::CpDiv(Complex<float>* nom, Complex<float>* denom, const int inpSize)
{
	Complex<float>* output;
	output = new Complex<float>[inpSize];
	for (int i = 0; i < inpSize; i++)
	{
		output[i] = nom[i] / denom[i];
	}
	return output;
}
/**
Performs complex division - dividing by real number, parametrs are passed by reference
P:
@param nom juce::dsp::Complex<float>, nominator
@param denom float, denominator
@param inpSize int, length of input signals (should be equal, not handled)
@return juce::dsp::Complex<float>
*/
Complex<float>* Compute::CpDiv(Complex<float>* nom, float denom, const int inpSize)
{
	ScopedPointer< Complex<float>> den;	//needs to be delted
	den = new Complex<float>(denom, 0.0);
	Complex<float>* output;
	output = new Complex<float>[inpSize];
	for (int i = 0; i < inpSize; i++)
	{
		output[i] = nom[i] / *den.get();
	}
	return output;
}
/**
Returns magnitude of passed complex signal.
P:
@param &inputfx juce::dsp::Complex<float>, complex signal
@param inpSize int, length of input signals (should be equal, not handled)
@return vector<float>, magnitude of complex input signal
*/
void Compute::CpToMagnitude(ScopedPointer< Complex<float>> &inputfx, float *irDataChannelWriter, const int inpSize)
{
	for (int i = 0; i < inpSize; i++)
	{
		irDataChannelWriter[i] = inputfx[i].real();
	}
}

//Lundeby's method
/**
Returns mean of passed signal, between passed start and stop index
P:
@param &inputfx vector<float>, input signal
@param StartIndex int, index from where mean will be calculated
@param StopIndex int, index to where mean will be calculated
@return float, value of mean
*/
float Compute::Mean(vector<float> &inputfx, const int StartIndex, const int StopIndex)
{
	float outp = 0.0;
	for (int i = StartIndex; i <= StopIndex; i++)
		outp += inputfx[i];
	return outp / (float)(StopIndex - StartIndex+1);
}
/**
Returns index of first value in signal, that is smaller than given value.
P:
@param &inputfx vector<float>, input signal
@param value float, value for comparison
@param StartIndex int, [optional] def = 0
@param StopIndex int, [optional] def = inputfx's length
@return int, index of value in input vector, that is first smaller than passed value
*/
int Compute::FirstLessThan(vector<float> &inputfx, const float value, int StartIndex , int StopIndex)
{
	if (StartIndex > StopIndex && StartIndex != 0)
	{
		return StartIndex - 1;
	}

	if (StopIndex == -1)
		StopIndex = inputfx.size()-1;
	int ResultIndex = StartIndex;
	vector<float>::iterator it1, it2;
	it1 = inputfx.begin() + StartIndex;		//iterator ukazuje na začátek, a když chci něco skipnout, tak se k tomu přičte startindex
	it2 = inputfx.begin() + StopIndex;		//stejně tak k začátku přičtu stopindex

	vector<float>::iterator it = find_if(it1, it2, [&](float i) {return i<value; });	/*vestavěná funkce, bere predikát, když tam dám [&] tak to hodnotu do funkce 
																						predikátu pošle referencí, float i - jakože to vezme z tu hodnotu float, 
																						na kterou ukazuje iterátor, kterej to prochází a tak se porovnává*/
	return it - inputfx.begin();		//jen zjistím, jak daleko je od začátku, šloby použít std::distance
}
/**
Returns index of first value in signal, that is smaller than given value. Based on regression line.
P:
@param Slope float, slope of regression line
@param Intercept float, intercept of regression line
@param value float, value
@return int, index of value in input vector, that is first smaller than passed value
*/
int Compute::FirstLessThan(float Slope, float Intercept, float value)
{
	return (int)(((value - Intercept) / Slope)+1);		//y = a*x +b, chci v jakým x budu mít hodnotu y, mám aji a, b. -> (y-b)/a = x a jestliže to chci vždycky zaokrouhlit nahoru, tak dám +1 a uříznu
}
/**
Returns last index that contains value greater than passed
P:
@param &inputfx vector<float>, input signal
@param value float, value
@param StartIndex int, [optional] def = 0
@param StopIndex int, [optional] def = inputfx's length
@return int, index of value in input vector, that is last greater than passed value
*/
int Compute::LastGreaterThan(std::vector<float> &inputfx, const float value, int StartIndex, int StopIndex)
{
	if (StopIndex == -1)
		StopIndex = inputfx.size() - 1;
	int ResultIndex = StartIndex;
	vector<float>::reverse_iterator it1, it2;
	it1 = inputfx.rbegin() + (inputfx.size() - 1 - StartIndex);		// rbegin ukazuje vlastně na konec a když se k tomu něco přičte, tak to jde opačným směrem, takže jestliže se chci dostat na Startindex, musím přičíst size-1-start
	it2 = inputfx.rbegin() + (inputfx.size() - 1 - StopIndex);		// to samý, prostě se potřebuju dostat na StopIndex, ale opčítá se to opačně
	vector<float>::reverse_iterator it = find_if(it2,it1, [&](float i) {return i>value; });		//reverse iterator počítá opačným směrem, takže i range musí být obráceně, proto it2,it1 v opačným pořadí
	return distance(it, inputfx.rend())-1;							//nakonec chci pozici od začátku celýho vektoru

}
/**
[OBSOLETE] - use MovingAverage
Performs local integration of passed signal
P:
@param &inputfx vector<float>, input signal
@param fs int, sample rate
@param interval int, interval of integration - either in ms or in samples
@param intervalIsInSamples bool, false - for passing it ms, true - for passing in number of samples
@return vector<float> locally integrated input
*/
vector<float> Compute::LocalIntegration(vector<float> &inputfx, const int fs, int interval, bool intervalIsInSamples)
{
	int intrv = interval;
	if(intervalIsInSamples == false)
		intrv = (int)(fs*interval / 1000.0);	//buď milisekundy přepočítám na samply, nebo ne, pokud je method true
	vector<float> outp(inputfx.size(),0);

	for (int i = 0; i < inputfx.size(); i += intrv)
	{
		if (i + intrv > inputfx.size())			//když bych už měl intervalem a iterací překročit délku signálu, tak ten poslední úsek vemu do konce
		{
			float localMean = Mean(inputfx, i, inputfx.size()-1);     //makes subarray - skips(n.of elements).takes(n.of elements)
			for (int j = i; j < inputfx.size(); j++)
				outp[j] = localMean;
		}

		else
		{
			float localMean = Mean(inputfx, i, i + intrv-1);
			for (int j = i; j < i + intrv; j++)
				outp[j] = localMean;

		}
	}
	return outp;
}
/**
Averages the given data with moving average filter.
P:
@param &inputfx vector<float>, input signal
@param fs int, sample rate
@param interval int, interval of average filter - either in ms or in samples - must be odd. If not, will be prolonged.
@param method bool, false - for passing it ms, true - for passing in number of samples
@return vector<float> locally integrated input
*/
std::vector<float> Compute::MovingAverage(vector<float> &inputfx, const int fs, int interval, bool intervalIsInSamples)
{
	int intrv;
	if (intervalIsInSamples == false)
	{
		intrv = (int)(fs*interval / 1000.0);	//buď milisekundy přepočítám na samply, nebo ne, pokud je method true
	}
	else
	{
		intrv = interval;
	}
	if (!(intrv % 2))		//it came out even
	{
		intrv += 1;
	}
	vector<float> outp(inputfx.size(), 0);

	for (int i = 0; i < outp.size(); ++i)
	{
		if(i< (intrv-1)/2)	//distance from start is smaller than half the average window
		{ 
			float tempsum = 0;
			for (int j = 0; j <= 2 * i; ++j)
			{
				tempsum += inputfx[j];
			}
			outp[i] = tempsum / float(2*i+1);
			continue;
		}
		else if ((outp.size() - 1) - i < (intrv - 1) / 2) //distance from end is smaller than half the average window
		{
			float tempsum = 0;
			for (int j = i - ((outp.size() - 1) - i); j <= outp.size() - 1; ++j)
			{
				tempsum += inputfx[j];
			}
			outp[i] = tempsum / float(2 * ((outp.size() - 1) - i) + 1);
			continue;
		}
		else
		{
			float tempsum = 0;
			for (int j = i - (intrv - 1) / 2; j <=i+(intrv - 1)/2; ++j)
			{
				tempsum += inputfx[j];
			}
			outp[i] = tempsum / float(intrv);
		}
	}
	return outp;
}

/**
Returns squarred array of input.
P:
@param &inputfx vector<float>, intput signal
@return vector<float>, squarred input
*/
vector<float> Compute::SquaredArr(const float * irDataChannelPointer, const int samples)
{
	vector<float> outp(samples,0.0);
	for (int i = 0; i < samples; i++)
		outp[i] = irDataChannelPointer[i]* irDataChannelPointer[i];
	return outp;
}
/**
Returns squarred array of input but input gets changed!
P:
@param &inputfx vector<float>, intput signal
@return vector<float>, squarred input
*/
vector<float> Compute::SquaredArrR(vector<float> &inputfx)
{
	for (int i = 0; i < inputfx.size(); i++)
		inputfx[i] = inputfx[i] * inputfx[i];
	return inputfx;

}
/**
Changes the value of input signal to dB and normalizes it to max value of input signal
P:
@param &inputfx vector<float>, input signal
@return vector<float>, output
*/
vector<float> Compute::Log10N(vector<float> &inputfx)
{	
	float maxval = *max_element(inputfx.begin(), inputfx.end());	//bere to iterátory a ty se standardně vytvoří tím begin a tak
	float minval = *min_element(inputfx.begin(), inputfx.end());	//navíc to chce ale to, na co to ukazuje, takže proto je předtím *
	if (abs(minval) > maxval)
		maxval = abs(minval);

	vector<float> outp;
	for (int i = 0; i < inputfx.size(); i++)
	{
		outp.push_back(10.0*log10((float)inputfx[i] / maxval));
		if (10.0*log10((float)inputfx[i] / maxval) == -INFINITY )
		{
			if (i < 0)
			{
				outp[i] = outp[i - 1];	//repeat the last
			}
			else
			{
				int j = 1;
				while (10.0*log10((float)inputfx[j] / maxval) == -INFINITY)		//find first non infinity value
				{
					j++;
				}
				outp[i] = 10.0*log10((float)inputfx[j] / maxval);
			}
		}
	}
	//auto new_end = remove_if(outp.begin(), outp.end(), [&](float i) {return i == -INFINITY; });		//pushes out elements that are -Inf
	//outp.erase(new_end, outp.end());		//previous doestn resize, so at the end, there are some old values, that need to be erased
	return outp;
}
/**
Changes the value of input signal to dB and normalizes it to max value of input signal. Changes the input!
P:
@param &inputfx vector<float>, input signal
@return vector<float>, output
*/
vector<float> Compute::Log10NR(vector<float> &inputfx)
{
	float maxval = *max_element(inputfx.begin(), inputfx.end());	//bere to iterátory a ty se standardně vytvoří tím begin a tak
	float minval = *min_element(inputfx.begin(), inputfx.end());	//navíc to chce ale to, na co to ukazuje, takže proto je předtím *
	if (abs(minval) > maxval)
		maxval = abs(minval);

	for (int i = 0; i < inputfx.size(); i++)
	{
		inputfx[i]= (10.0*log10((float)inputfx[i] / maxval));
	}

	auto new_end = remove_if(inputfx.begin(), inputfx.end(), [&](float i) {return i == -INFINITY; });		//pushes out elements that are -Inf
	inputfx.erase(new_end, inputfx.end());		//previous doestn resize, so at the end, there are some old values, that need to be erased
	return inputfx;
}
/**
Returns dynamic range of passed signal - max-min.
P:
@param &inputfx vector<float>, input signal
@param StartIndex int, [optional] def = 0
@param StopIndex int, [optional] def = inputfx's length
@return float, dynamic range
*/
float Compute::DynRange(vector<float> &inputfx, int StartIndex, int StopIndex)
{
	if (StopIndex == -1)
		StopIndex = inputfx.size();
	float maxvalue = -INFINITY, minvalue = INFINITY;	//nejdřív nastavim limity obráceně a pak je posouvám
	for (int i = StartIndex; i < StopIndex; i++)
	{
		if (inputfx[i] <= minvalue)
			minvalue = inputfx[i];
		if (inputfx[i] >= maxvalue)
			maxvalue = inputfx[i];
	}
	return abs(maxvalue - minvalue);
}
/**
Returns regression parametrs of line that fit best into passed data.
P:
@param &x vector<float>, x values of signal
@param &y vector<float>, y values of signal
@param StartIndex int, [optional] def = 0
@param StopIndex int, [optional] def = inputfx's length
@return DecayParameters - struct that contains 2 fields
.Slope
.Intercept
*/
DecayParameters Compute::LinLSSolver(vector<float> & x, vector<float> & y, int StartIndex, int StopIndex)
//https://people.sc.fsu.edu/~jburkardt/cpp_src/llsq/llsq.html
/**
****************************************************************************80

  Purpose:

    LLSQ solves a linear least squares problem matching a line to data.

  Discussion:

    A formula for a line of the form Y = A * X + B is sought, which
    will minimize the root-mean-square error to N data points ( X[I], Y[I] );

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    17 July 2011

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of data values.

    Input, double X[N], Y[N], the coordinates of the data points.

    Output, double &A, &B, the slope and Y-intercept of the least-squares
    approximant to the data.
*/
{
	DecayParameters result;
	
	double bot;
	int i;
	double top;
	double xbar;
	double ybar;
	
	if (x.size() != y.size())
		throw "Input vectors are not the same sizes!";
	if (StopIndex == -1)
		StopIndex = x.size() - 1;

	int n = StopIndex - StartIndex + 1;


	//
	//  Average X and Y.
	//
	xbar = 0.0;
	ybar = 0.0;
	for (i = StartIndex; i < StopIndex+1; i++)
	{
		xbar = xbar + x[i];
		ybar = ybar + y[i];
	}
	xbar = xbar / (double)n;
	ybar = ybar / (double)n;
	//
	//  Compute Beta.
	//
	top = 0.0;
	bot = 0.0;
	for (i = StartIndex; i < StopIndex+1; i++)
	{
		top = top + (x[i] - xbar) * (y[i] - ybar);
		bot = bot + (x[i] - xbar) * (x[i] - xbar);
	}
	result.Slope = top / bot;
	result.Intercept = ybar - result.Slope * xbar;

	return result;
}


//Schroeder's integral
/**
Performs backward integration of input signal
P:
@param &inputfx vector<float>, input signal
@return vector<float>, output
*/
vector<float> Compute::ReversedSum(vector<float> &inputfx, int predelay)
{
	vector<float> outpfx(inputfx.size(), 0.0);
	if (predelay > inputfx.size())
	{
		predelay = inputfx.size() - 1;
	}
	outpfx[inputfx.size() - 1] = inputfx[inputfx.size() - 1];
	for (int i = inputfx.size() - 2; i>predelay-1; --i)
	{
		outpfx[i] = inputfx[i] + outpfx[i + 1];						//a pak jdu od konce a vždycky o jedno bližší k začátku je input toho samýho indexu + output předchozího co jsem zapsal
	}
	for (int i = predelay-1; i > -1; --i)
	{
		outpfx[i] = outpfx[i + 1];									// abych skončil integraci v max hodnotě a pak nepřičítal případný předcházející šum
	}
	return outpfx;
}


#pragma endregion

