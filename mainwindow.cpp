#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <cmath>
#include <AudioFile.h>
#include <Windows.h>
#include <MMsystem.h>
#include <algorithm>
#include <stdlib.h>

#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QLineEdit>

float gain;
bool multiple_filters_allowed;

int echo_repeats;
int echo_seconds_delay;
double echo_gain;

int fadeOut_time;
int fadeIn_time;
//=======================================================================
namespace examples
{
    void loadAudioFileAndPrintSummary();
    void loadAudioFileAndAplify();
    void loadAudioFileAndReverse();

    void loadAudioFileAndSaveIt(std::string fileNameSave);
    void loadAudioFileAndIncreaseVolume();
    void LoadAudioFikeAndDecreaseVolume();

    void loadChoosenAudioFileAndPrintSummary(std::string directory, std::string fileName);

    std::string original_directory;
    std::string original_fileName;

    std::string processed_filename;
    std::string FilenNameToSave;
    std::string fileNameChanged_all;

    //void playChangedAudioFile(std::string fileNamePlay);
    void loadAudioFileAndFadeIn(float fadetime);
    void loadAudioFileAndFadeOut(float fadetime);
}

//=======================================================================
namespace examples
{

    //=======================================================================
    void loadAudioFileAndPrintSummary()
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and Print Summary" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;

        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string filePath = std::string (original_directory + "/" + original_fileName);

        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file

        AudioFile<float> a;
        bool loadedOK = a.load (filePath);

        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);

        //---------------------------------------------------------------
        // 3. Let's print out some key details

        std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
        std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
        std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
        std::cout << "Length in Seconds: " << a.getLengthInSeconds() << std::endl;
        std::cout << std::endl;
    }

    //=======================================================================
    void loadAudioFileAndAplify()
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and AMPLIFY Samples" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;

        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        std::string inputFilePath;
        if(!multiple_filters_allowed)
        {
            inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + original_fileName;
        }
        else
        {
            inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + processed_filename;
        }

        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file

        AudioFile<float> a;
        bool loadedOK = a.load (inputFilePath);

        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);

        //---------------------------------------------------------------
        // 3. Let's apply a gain to every audio sample

        gain = 1e-3 * std::exp(6.908 * (gain/1000));
        qInfo() << "C++ Style Info Message";

        if (gain < 0){gain = 0;}

        float abs_max = 0;

        for (int i = 0; i < a.getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < a.getNumChannels(); channel++)
            {
                if (abs(a.samples[channel][i]) > abs_max)
                {
                    abs_max = abs(a.samples[channel][i]);
                }
            }
        }

        float max_gain = 1/abs_max;

        if(gain > max_gain){gain = max_gain;}

        for (int i = 0; i < a.getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < a.getNumChannels(); channel++)
            {
                a.samples[channel][i] = a.samples[channel][i] * gain;
            }
        }

        // 4. Write audio file to disk


        std::string outputFilePath = "temp.wav"; // change this to somewhere useful for you
        a.save (outputFilePath, AudioFileFormat::Wave);
        FilenNameToSave = outputFilePath;

//        wchar_t sound_filename[256];
//        const char * c = outputFilePath.c_str();
//        const size_t cSize = strlen(c)+1;
//        wchar_t* wc = new wchar_t[cSize];
//        std::mbstowcs (wc, c, cSize);

//        PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);

        processed_filename = outputFilePath;
        //if(multiple_filters_allowed){processed_filename = outputFilePath;}
    }

    //=======================================================================

    void loadAudioFileAndFadeIn(float fadetime)
        {
            //---------------------------------------------------------------
            std::cout << "**********************" << std::endl;
            std::cout << "Running Example: Load Audio File and Fade In Samples" << std::endl;
            std::cout << "**********************" << std::endl << std::endl;

            //---------------------------------------------------------------
            // 1. Set a file path to an audio file on your machine
            std::string inputFilePath;
            if(!multiple_filters_allowed)
            {
                inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + original_fileName;
            }
            else
            {
                inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + processed_filename;
            }

            //---------------------------------------------------------------
            // 2. Create an AudioFile object and load the audio file

            AudioFile<float> a;
            bool loadedOK = a.load (inputFilePath);

            /** If you hit this assert then the file path above
             probably doesn't refer to a valid audio file */
            assert (loadedOK);

            //---------------------------------------------------------------
            // 3. Let's apply a gain to every audio sample

            float outputGain = 0.0f;
            //float fadeTime = 0.5f;
            float fadeTime = fadetime/(a.getLengthInSeconds());

            qInfo() << "C++ Style Info Message";

            if(fadeTime > 1.0) fadeTime = 1.0f;
            if(fadeTime <= 0) fadeTime = 0.1f;

            float fadeInStop = fadeTime * a.getNumSamplesPerChannel();
            float gainProgressionPerSample = 1.0f/fadeInStop;

            for (int i = 0; i < a.getNumSamplesPerChannel(); i++)
            {
                for (int channel = 0; channel < a.getNumChannels(); channel++)
                {
                    if (i < fadeInStop)
                    {
                      outputGain += gainProgressionPerSample;
                      if(outputGain > 1){outputGain = 1;}
                    }
                    a.samples[channel][i] = a.samples[channel][i] * outputGain;
                }
            }

            // 4. Write audio file to disk


            std::string outputFilePath = "temp.wav"; // change this to somewhere useful for you
            a.save (outputFilePath, AudioFileFormat::Wave);
            FilenNameToSave = outputFilePath;

//            wchar_t sound_filename[256];
//            const char * c = outputFilePath.c_str();
//            const size_t cSize = strlen(c)+1;
//            wchar_t* wc = new wchar_t[cSize];
//            std::mbstowcs (wc, c, cSize);

            //PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);

            processed_filename = outputFilePath;
        }
    //--------------------------------------------------------------------------

    //=======================================================================
      void loadAudioFileAndFadeOut(float fadetime)
      {
          //---------------------------------------------------------------
          std::cout << "**********************" << std::endl;
          std::cout << "Running Example: Load Audio File and Fade Out Samples" << std::endl;
          std::cout << "**********************" << std::endl << std::endl;

          //---------------------------------------------------------------
          // 1. Set a file path to an audio file on your machine
          std::string inputFilePath;
          if(!multiple_filters_allowed)
          {
              inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + original_fileName;
          }
          else
          {
              inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + processed_filename;
          }

          //---------------------------------------------------------------
          // 2. Create an AudioFile object and load the audio file

          AudioFile<float> a;
          bool loadedOK = a.load (inputFilePath);

          /** If you hit this assert then the file path above
           probably doesn't refer to a valid audio file */
          assert (loadedOK);

          //---------------------------------------------------------------
          // 3. Let's apply a gain to every audio sample

          float outputGain = 1;
          //float fadeTime = 0.5f;
          float fadeTime = fadetime/(a.getLengthInSeconds());

          qInfo() << "C++ Style Info Message";

          if(fadeTime > 1.0) fadeTime = 1.0f;
          if(fadeTime <= 0) fadeTime = 0.1f;

          float gainProgressionPerSample = 1.0f/(fadeTime * a.getNumSamplesPerChannel());
          float fadeOutBegin = a.getNumSamplesPerChannel() - (fadeTime * a.getNumSamplesPerChannel());

          for (int i = fadeOutBegin; i < a.getNumSamplesPerChannel(); i++)
          {
              for (int channel = 0; channel < a.getNumChannels(); channel++)
              {
                  outputGain -= gainProgressionPerSample;
                  if (outputGain < 0) {outputGain = 0;}
                  a.samples[channel][i] = a.samples[channel][i] * outputGain;
              }
          }


          // 4. Write audio file to disk


          std::string outputFilePath = "temp.wav"; // change this to somewhere useful for you
          a.save (outputFilePath, AudioFileFormat::Wave);
          FilenNameToSave = outputFilePath;

//          wchar_t sound_filename[256];
//          const char * c = outputFilePath.c_str();
//          const size_t cSize = strlen(c)+1;
//          wchar_t* wc = new wchar_t[cSize];
//          std::mbstowcs (wc, c, cSize);

          //PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);

          processed_filename = outputFilePath;
      }
    //-------------------------------------------------------------------------------
        void loadAudioFileAndReverse()
        {
            //---------------------------------------------------------------
            std::cout << "**********************" << std::endl;
            std::cout << "Running Example: Load Audio File and REVERSE" << std::endl;
            std::cout << "**********************" << std::endl << std::endl;

            //---------------------------------------------------------------
            // 1. Set a file path to an audio file on your machine
            std::string inputFilePath;
            if(!multiple_filters_allowed)
            {
                inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + original_fileName;
            }
            else
            {
                inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + processed_filename;
            }
            //---------------------------------------------------------------
            // 2. Create an AudioFile object and load the audio file

            AudioFile<float> a;
            bool loadedOK = a.load (inputFilePath);

            /** If you hit this assert then the file path above
             probably doesn't refer to a valid audio file */
            assert (loadedOK);

            //---------------------------------------------------------------
            // 3. Let's apply a reverse

            std::reverse(a.samples[0].begin(), a.samples[0].end());
            std::reverse(a.samples[1].begin(), a.samples[1].end());

            //---------------------------------------------------------------
            // 4. Write audio file to disk

            std::string outputFilePath = "temp.wav"; // change this to somewhere useful for you
            a.save (outputFilePath, AudioFileFormat::Wave);
            FilenNameToSave = outputFilePath;

//            wchar_t sound_filename[256];
//            const char * c = outputFilePath.c_str();
//            const size_t cSize = strlen(c)+1;
//            wchar_t* wc = new wchar_t[cSize];
//            std::mbstowcs (wc, c, cSize);


            //swprintf(fileName, sizeof(fileName), L"%hs", sound_filename);

            //PlaySound(TEXT("choosen_loaded_file.wav"), NULL, SND_ASYNC);
            //PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);

            processed_filename = outputFilePath;
            //if(multiple_filters_allowed){original_fileName = outputFilePath;}
        }

//-----------------------------------------------------------------------------------
//        void playChangedAudioFile(std::string fileNamePlay)
//            {
//                //---------------------------------------------------------------
//                std::cout << "**********************" << std::endl;
//                std::cout << "Runnning Example: Load Audio File and Print Summary" << std::endl;
//                std::cout << "**********************" << std::endl << std::endl;

//                //---------------------------------------------------------------
//                // 1. Set a file path to an audio file on your machine
//                const std::string filePath =  std::string (PROJECT_BINARY_DIR) + "/" + fileNamePlay;

//                std::cout << "To jest filePath" << std::endl;
//                std::cout << filePath << std::endl;

//                //---------------------------------------------------------------
//                // 2. Create an AudioFile object and load the audio file

//                AudioFile<float> a;
//                bool loadedOK = a.load (filePath);

//                /** If you hit this assert then the file path above
//                 probably doesn't refer to a valid audio file */
//                assert (loadedOK);

//                //---------------------------------------------------------------
//                // 3. Let's print out some key details

//                std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
//                std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
//                std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
//                std::cout << "Length in Seconds: " << a.getLengthInSeconds() << std::endl;
//                std::cout << std::endl;

//                //---------------------------------------------------------------
//                // 4. Write audio file to disk

//                std::string outputFilePath = fileNamePlay; // change this to somewhere useful for you

//                wchar_t sound_filename[256];
//                const char * c = outputFilePath.c_str();
//                const size_t cSize = strlen(c)+1;
//                wchar_t* wc = new wchar_t[cSize];
//                std::mbstowcs (wc, c, cSize);


//                //swprintf(fileName, sizeof(fileName), L"%hs", sound_filename);

//                //PlaySound(TEXT("choosen_loaded_file.wav"), NULL, SND_ASYNC);
//                PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);
//            }


    //=======================================================================
    void loadAudioFileAndSaveIt(std::string fileNameSave)
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and SAVE It" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;

        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + FilenNameToSave;

        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file

        AudioFile<float> a;
        bool loadedOK = a.load (inputFilePath);

        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);

        //---------------------------------------------------------------
        // 4. Write audio file to disk

        std::string outputFilePath = fileNameSave; // zapisuje pod wybraną ścieżką
        a.save (outputFilePath, AudioFileFormat::Wave);
    }


    //=======================================================================

    //kopiuje wskazany plik do folderu roboczego, analizuje go i gra muzyke
    void loadChoosenAudioFileAndPrintSummary(std::string directory, std::string fileName)
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Runnning Example: Load Audio File and Print Summary" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;

        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string filePath = original_directory + "/" + original_fileName;

        std::cout << "To jest filePath" << std::endl;
        std::cout << filePath << std::endl;

        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file

        AudioFile<float> a;
        bool loadedOK = a.load (filePath);

        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);

        //---------------------------------------------------------------
        // 3. Let's print out some key details

        std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
        std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
        std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
        std::cout << "Length in Seconds: " << a.getLengthInSeconds() << std::endl;
        std::cout << std::endl;

        //---------------------------------------------------------------
        // 4. Save the AudioFile

        //zapisanie pod tą samą nazwą
        std::string filePath_save = std::string (PROJECT_BINARY_DIR) + + "/" + original_fileName; // change this to somewhere useful for you
        a.save (filePath_save, AudioFileFormat::Wave);
    }
    //=======================================================================
    void add_echoes()
    {
        assert(echo_seconds_delay != 0);
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and ECHO" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;

        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        std::string inputFilePath;
        if(!multiple_filters_allowed)
        {
            inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + original_fileName;
        }
        else
        {
            inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + processed_filename;
        }

        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file

        AudioFile<float> a;
        bool loadedOK = a.load (inputFilePath);

        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);

        int sound_samples = a.getNumSamplesPerChannel();
        int sample_rate = a.getSampleRate();

        int k = 0;
        int delay_samples = echo_seconds_delay * sample_rate;
        int number_of_repeats = a.getLengthInSeconds() / echo_seconds_delay;

        for(int j = 1; j < number_of_repeats; j++)
        {
            for (int i = j * delay_samples; i < a.getNumSamplesPerChannel(); i++, k++)
            {
                for (int channel = 0; channel < a.getNumChannels(); channel++)
                {
                    a.samples[channel][i] = a.samples[channel][i] + a.samples[channel][k] * pow(echo_gain,j);
                    if(a.samples[channel][i] > 1){a.samples[channel][i] = 1;}
                    else if(a.samples[channel][i] < -1){a.samples[channel][i] = -1;}
                }
            }
            k = 0;
        }

        // 4. Write audio file to disk

        std::string outputFilePath = "temp.wav"; // change this to somewhere useful for you
        a.save (outputFilePath, AudioFileFormat::Wave);
        FilenNameToSave = outputFilePath;

//        wchar_t sound_filename[256];
//        const char * c = outputFilePath.c_str();
//        const size_t cSize = strlen(c)+1;
//        wchar_t* wc = new wchar_t[cSize];
//        std::mbstowcs (wc, c, cSize);
//        PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);

        processed_filename = outputFilePath;
    }
    void changeSpeedAudioFile(float frequency)
        {

            //---------------------------------------------------------------
            std::cout << "**********************" << std::endl;
            std::cout << "Running Example: Load Audio File and change speed" << std::endl;
            std::cout << "**********************" << std::endl << std::endl;

            //---------------------------------------------------------------
            // 1. Set a file path to an audio file on your machine
            std::string inputFilePath;
            if(!multiple_filters_allowed)
            {
                inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + original_fileName;
            }
            else
            {
                inputFilePath = std::string (PROJECT_BINARY_DIR) + "/" + processed_filename;
            }

            //---------------------------------------------------------------
            // 2. Create an AudioFile object and load the audio file

            AudioFile<float> a;
            bool loadedOK = a.load (inputFilePath);

            /** If you hit this assert then the file path above
             probably doesn't refer to a valid audio file */
            assert (loadedOK);

            assert(frequency > 0);
            int size_change = a.getNumSamplesPerChannel()/frequency;

            AudioFile<float> b;
            bool loadedOK_b = b.load (inputFilePath);
            assert (loadedOK_b);

            b.setNumSamplesPerChannel(size_change);

            float j = frequency;

            for (int i = 0; i < size_change; i++)
            {
                for (int channel = 0; channel < a.getNumChannels(); channel++)
                {
                    if(i == 0) b.samples[channel][i] = a.samples[channel][0];

                    if(i*j < a.getNumSamplesPerChannel()) {
                        b.samples[channel][i] = a.samples[channel][j*i];
                    }
                }
            }

            // 4. Write audio file to disk

            std::string outputFilePath = "temp.wav"; // change this to somewhere useful for you
            b.save (outputFilePath, AudioFileFormat::Wave);
            FilenNameToSave = outputFilePath;

//            wchar_t sound_filename[256];
//            const char * c = outputFilePath.c_str();
//            const size_t cSize = strlen(c)+1;
//            wchar_t* wc = new wchar_t[cSize];
//            std::mbstowcs (wc, c, cSize);
//            PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC); // odegranie filtra

            processed_filename = outputFilePath;
        }


} // tu sie konczy 'example'



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}

//---------------------------------------------------------------

//---------------------------------------------------------------
// Wczytuje po wciśnięciu przycisku "ClickME" zapisuje ścieżke do pliku
std::string MainWindow::on_Button_directory_clicked()
{

    //---------------------------------------------------------------
    // 1. Zapisanie ścieżki do pliku (z piewszego okienka tekstowego)

    //ui->lineEdit->setText("hello world");
    QMessageBox::information(this,"Title",ui->lineEdit->text());
    QString i = ui->lineEdit->text();
    //std::cout << ui->text() << std::endl;
    qDebug(qUtf8Printable(i));

    //---------------------------------------------------------------

    //---------------------------------------------------------------
    // 2. Sprawdzenie poprawności zapisania ścieżki do pliku (z piewszego okienka tekstowego)

    std::cout << "To jest zapisana dirctory jako Qstring" << std::endl;
    std::string directory = i.toStdString();
    std::cout << i.toStdString() << std::endl;
    std::cout << "To jest zapisana dirctory jako string" << std::endl;
    std::cout << directory << std::endl;

    //---------------------------------------------------------------

    //---------------------------------------------------------------
    // 3. Zapisanie nazwy pliku (z drugiego okienka tekstowego)

    QMessageBox::information(this,"Title",ui->lineEdit_2->text());
    QString j = ui->lineEdit_2->text();
    //std::cout << ui->text() << std::endl;
    qDebug(qUtf8Printable(j));

    //---------------------------------------------------------------

    //---------------------------------------------------------------
    // 4. Sprawdzenie poprawności zapisania nazwy pliku (z piewszego okienka tekstowego)

    std::cout << "To jest zapisana nazwa pliku jako Qstring" << std::endl;
    std::string fileName = j.toStdString();
    std::cout << j.toStdString() << std::endl;
    std::cout << "To jest zapisana nazwa pliku jako string" << std::endl;
    std::cout << fileName << std::endl;

    //---------------------------------------------------------------

    examples::original_directory = directory; // zapisanie directory do pliku
    examples::original_fileName = fileName; // zapisanie directory do pliku
    examples::FilenNameToSave = fileName; //dla funkcji save
    examples::processed_filename = fileName;

//    std::cout << "To jest zapisana nazwa jako fileName_all" << std::endl;
//    std::cout << examples::original_fileName << std::endl;


    examples::loadChoosenAudioFileAndPrintSummary(directory, fileName);

//    wchar_t sound_filename[256];
//    const char * c = fileName.c_str();
//    const size_t cSize = strlen(c)+1;
//    wchar_t* wc = new wchar_t[cSize];
//    std::mbstowcs (wc, c, cSize);

//    PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);
    return directory;
}

//--------------------------------------------------------------------
void MainWindow::on_Button_save_clicked()
{
    //---------------------------------------------------------------
    // 1. Zapisanie nazwy pliku (z trzeciego okienka tekstowego)

    QMessageBox::information(this,"Title",ui->lineEdit_3->text());
    QString j = ui->lineEdit_3->text();
    //std::cout << ui->text() << std::endl;
    qDebug(qUtf8Printable(j));

    std::string fileNameSave = j.toStdString();

    examples::loadAudioFileAndSaveIt(fileNameSave);

    //---------------------------------------------------------------
}

//---------------------------------------------------------------

//--------------------REVERSE-------------------

void MainWindow::on_pushButton_3_clicked()
{
    //---------------------------------------------------------------
    /** Loads an audio file and processess the samples */
    examples::loadAudioFileAndReverse();
}

//---------------------------------------------------------------

//--------------------AMPLIFY-------------------

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    gain = value;
}

void MainWindow::on_pushButton_4_clicked()
{
    //---------------------------------------------------------------
    /** Loads an audio file and processess the samples */
    examples::loadAudioFileAndAplify();
}

//---------------------------------------------------------------

void MainWindow::on_checkBox_toggled(bool checked)
{
    multiple_filters_allowed = checked;
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    echo_repeats = arg1;
}


void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    echo_seconds_delay = arg1;
}


void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    echo_gain = arg1;
}


void MainWindow::on_pushButton_6_clicked()
{
    examples::add_echoes();
}
void MainWindow::on_pushButton_fadeIN_clicked(){

    QMessageBox::information(this,"Title",ui->lineEdit_fadeIN->text());
    QString j = ui->lineEdit_fadeIN->text();
    //std::cout << ui->text() << std::endl;
    qDebug(qUtf8Printable(j));
    float fade_time = j.toFloat();

    std::cout << "To jest fade time" << std::endl;
    std::cout << fade_time << std::endl;

    examples::loadAudioFileAndFadeIn(fade_time); // wykonaj fade in
}



void MainWindow::on_pushButton_fadeOUT_clicked(){
    QMessageBox::information(this,"Title",ui->lineEdit_fadeOUT->text());
    QString j = ui->lineEdit_fadeOUT->text();
    //std::cout << ui->text() << std::endl;
    qDebug(qUtf8Printable(j));
    float fade_time = j.toFloat();

    examples::loadAudioFileAndFadeOut(fade_time); // wykonaj fade out
}



void MainWindow::on_pushButton_clicked()
{
        // 1. Odtworzenie pliku oryginalnego
        std::string outputFilePath = examples::original_fileName; // change this to somewhere useful for you

        wchar_t sound_filename[256];
        const char * c = outputFilePath.c_str();
        const size_t cSize = strlen(c)+1;
        wchar_t* wc = new wchar_t[cSize];
        std::mbstowcs (wc, c, cSize);
        PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);
}


void MainWindow::on_pushButton_5_clicked()
{
    // 1. Odtworzenie pliku przetworzonego
    std::string outputFilePath = "temp.wav"; // change this to somewhere useful for you

    wchar_t sound_filename[256];
    const char * c = outputFilePath.c_str();
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    std::mbstowcs (wc, c, cSize);
    PlaySound(wc, NULL, SND_FILENAME | SND_ASYNC);
}


void MainWindow::on_pushButton_8_clicked()
{
    QMessageBox::information(this,"Title",ui-> lineEdit_4->text());
    QString j = ui-> lineEdit_4->text();
    //std::cout << ui->text() << std::endl;
    qDebug(qUtf8Printable(j));
    float frequency = j.toFloat();

    examples::changeSpeedAudioFile(frequency); // wykonaj change speed
}

