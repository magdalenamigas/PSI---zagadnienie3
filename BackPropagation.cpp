#include<iostream>
#include<math.h>
#include<cstdlib>
#include<fstream>
using namespace std;

class SiecNeronowa {

public:
	class Warstwa {
	public:
		int iloscWejsc; // neurony w warstwie poprzedniej
		int iloscWyjsc; // neurony w warstwie obecnej
		float *wyjscia;
		float *wejscia;
		float **wagi;
		float **deltaWag;
		float *gamma;
		float *blad;

		Warstwa(int inp, int out) {

			this->iloscWejsc = inp;
			this->iloscWyjsc = out;

			wyjscia = new float[out];
			wejscia = new float[inp];

			wagi = new float *[out];
			for (int i = 0; i < out; i++)
				wagi[i] = new float[inp];

			deltaWag = new float *[out];
			for (int i = 0; i < out; i++)
				deltaWag[i] = new float[inp];

			gamma = new float[out];
			blad = new float[out];
			Inicjuj_wagi();
		}
		Warstwa() {}

		void Inicjuj_wagi() {

			for (int i = 0; i < iloscWyjsc; i++)
			{
				for (int j = 0; j < iloscWejsc; j++)
				{
					wagi[i][j] = 0.5 + (double)rand() / RAND_MAX* (-1);
				}
			}
		}

		float* Licz_w_przod(float *in)
		{
			this->wejscia = in;
			for (int i = 0; i < iloscWyjsc; i++)
			{
				wyjscia[i] = 0;
				for (int j = 0; j < iloscWejsc;j++)
				{
					wyjscia[i] += wejscia[j] * wagi[i][j];
				}
				wyjscia[i] = tanh(wyjscia[i]);
			}
			return wyjscia;
		}

		float pochodnaTan(float value)
		{
			return (1 - (value*value));
		}

		void propaguj_wyjscie(float *expected) {

			for (int i = 0; i < iloscWyjsc; i++)
				blad[i] = wyjscia[i] - expected[i];

			for (int i = 0; i < iloscWyjsc; i++)
				gamma[i] = blad[i] * pochodnaTan(wyjscia[i]);

			for (int i = 0; i < iloscWyjsc; i++) {
				for (int j = 0; j < iloscWejsc; j++)
				{
					deltaWag[i][j] = gamma[i] * wejscia[j];
				}
			}
		}

		void propaguj_ukryte(float *gammaForward, int size2, float** weightsForward) {

			for (int i = 0; i < iloscWyjsc; i++)
			{
				gamma[i] = 0;

				for (int j = 0; j < size2; j++)
				{
					gamma[i] += gammaForward[j] * weightsForward[j][i];
				}
				gamma[i] *= pochodnaTan(wyjscia[i]);
			}


			for (int i = 0; i < iloscWyjsc; i++) {
				for (int j = 0; j < iloscWejsc; j++)
				{
					deltaWag[i][j] = gamma[i] * wejscia[j];
				}
			}
		}
		void aktualizuj_wagi() {

			for (int i = 0; i < iloscWyjsc; i++)
			{
				for (int j = 0; j < iloscWejsc; j++)
				{
					wagi[i][j] -= deltaWag[i][j] * 0.4;
				}
			}
		}
	};

	int *warstwa;
	int rozmiar;
	Warstwa *warstwy;

	float* LiczWPrzod(float *inputs)
	{
		warstwy[0].Licz_w_przod(inputs);

		for (int i = 1; i < rozmiar - 1; i++)
		{
			warstwy[i].Licz_w_przod(warstwy[i - 1].wyjscia);
		}

		return warstwy[rozmiar - 2].wyjscia;
	}

	void LiczWTyl(float *expected) {

		for (int i = rozmiar - 2; i >= 0; i--)
		{
			if (i == rozmiar - 2)
			{
				warstwy[i].propaguj_wyjscie(expected);
			}
			else {
				warstwy[i].propaguj_ukryte(warstwy[i + 1].gamma, warstwy[i + 1].iloscWyjsc, warstwy[i + 1].wagi);
			}
		}
		for (int i = 0; i < rozmiar - 1; i++)
		{
			warstwy[i].aktualizuj_wagi();
		}
	}

	SiecNeronowa(int *war, int rozm) {

		this->rozmiar = rozm;
		this->warstwa = new int[rozm];
		for (int i = 0; i < rozm;i++)
		{
			this->warstwa[i] = war[i];
		}

		this->warstwy = new Warstwa[rozm - 1];
		cout << "Warstwa wejsciowa: 35 neuronow " << endl;
		for (int i = 0; i < rozm-1; i++)
		{
			Warstwa *A=new Warstwa(warstwa[i], warstwa[i + 1]);
			warstwy[i] = *A;
			if (i == rozm - 2)
			{
				cout << "Warstwa wyjsciowa: 20 neuronow " << endl;
				break;
			}
			cout << "Warstwa ukryta nr " << i+1 << ": " << A->iloscWyjsc <<" neronow" << endl;
		}
	}

	void Testuj_siec(float* wejscia, char litera)
	{
		char literki_tab[20] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'a', 'b', 'c', 'd', 'n', 'f', 'h', 'i', 'k', 'o' };
		int index = 0;
		for (int i = 0;i<20;i++)
		{
			if (literki_tab[i] == litera) index = i;
		}
		int n = 20;
		float *wynik = new float[n];
		wynik = LiczWPrzod(wejscia);
		cout << "Czy otrzymana litera to " << litera << " :" << wynik[index] << endl;
	}
	SiecNeronowa() {}
};

class ZestawDanych {

public:
	int ilosc;
	float *Wzor;
	float *Wynik;

	ZestawDanych() {
		this->ilosc = 20;
		int n = 35;
		int m = 20;
		Wzor = new float[n];
		Wynik = new float[m];
	}
};

int main()
{
	int n = 4;
	int* tab = new int[n];
	tab[0] = 35;
	tab[1] = 35;
	tab[2] = 35;
	tab[3] = 20;
	SiecNeronowa *MLP=new SiecNeronowa(tab, 4);
	ZestawDanych letters[20];
	fstream plik;
	plik.open("zbior_uczacy1.txt");
	if (plik.good())
	{

		for (int i = 0; i<20;i++)
		{
			for (int j = 0; j<35; j++)
			{
				plik >> letters[i].Wzor[j];
			}

			for (int k = 0; k<20; k++)
			{
				plik >> letters[i].Wynik[k];
			}
		}
	}
	else
	{
		cout << "blad otwarcia pliku!";
	}

	for (int i = 0; i < 500; i++)
	{
		for (int j = 0;j< 20;j++)
		{
			MLP->LiczWPrzod(letters[j].Wzor);
			MLP->LiczWTyl(letters[j].Wynik);
		}
	}

	float test[35]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0 }; // o
	float test2[35] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 ,1 ,0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 ,0 ,0 ,0, 0, 0, 0 }; // I
	float test3[35] = { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0 ,0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 }; // C
	MLP->Testuj_siec(test,'o');
	MLP->Testuj_siec(test2, 'I');
	MLP->Testuj_siec(test3, 'C');
	getchar();
}
