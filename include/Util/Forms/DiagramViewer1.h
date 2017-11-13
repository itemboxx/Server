#pragma once
#include <vector>
#include "C:\Home\Util\FuncDPoint.h"
namespace Forms {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for DiagramViewer
	/// </summary>
	public ref class DiagramViewer : public System::Windows::Forms::Form
	{
	public:
		DiagramViewer(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DiagramViewer()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^  chartControl;

	protected:


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^  series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::DataPoint^  dataPoint1 = (gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(0.1,
				0.1));
			System::Windows::Forms::DataVisualization::Charting::DataPoint^  dataPoint2 = (gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(0.1,
				0.3));
			System::Windows::Forms::DataVisualization::Charting::DataPoint^  dataPoint3 = (gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(0.2,
				0.5));
			System::Windows::Forms::DataVisualization::Charting::DataPoint^  dataPoint4 = (gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(0,
				0.2));
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->chartControl = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->tableLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chartControl))->BeginInit();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel1->Controls->Add(this->chartControl, 0, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(885, 699);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// chartControl
			// 
			chartArea1->Name = L"ChartArea1";
			this->chartControl->ChartAreas->Add(chartArea1);
			this->tableLayoutPanel1->SetColumnSpan(this->chartControl, 2);
			this->chartControl->Dock = System::Windows::Forms::DockStyle::Fill;
			legend1->Name = L"Legend1";
			this->chartControl->Legends->Add(legend1);
			this->chartControl->Location = System::Drawing::Point(3, 3);
			this->chartControl->Name = L"chartControl";
			series1->ChartArea = L"ChartArea1";
			series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series1->Legend = L"Legend1";
			series1->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Square;
			series1->Name = L"Series1";
			series1->Points->Add(dataPoint1);
			series1->Points->Add(dataPoint2);
			series1->Points->Add(dataPoint3);
			series1->Points->Add(dataPoint4);
			this->chartControl->Series->Add(series1);
			this->chartControl->Size = System::Drawing::Size(879, 653);
			this->chartControl->TabIndex = 0;
			this->chartControl->Text = L"chart1";
			// 
			// DiagramViewer
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(12, 25);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(885, 699);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Name = L"DiagramViewer";
			this->Text = L"DiagramViewer";
			this->tableLayoutPanel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chartControl))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	public: void Show(std::vector<float>& vals){

		chartControl->Series->Clear();
		Forms::DataVisualization::Charting::Series^ series1 = gcnew Forms::DataVisualization::Charting::Series();


		series1->Name = "Series1";
		series1->Color = System::Drawing::Color::Green;
		series1->IsVisibleInLegend = false;
		series1->IsXValueIndexed = true;
		series1->ChartType = Forms::DataVisualization::Charting::SeriesChartType::Line;


		chartControl->Series->Add(series1);
		int pointsCount = vals.size();
		for (int i = 0; i < pointsCount; i++)
		{
			series1->Points->AddXY(i, vals[i]);
		}
		chartControl->Invalidate();
		Show();
	}
			void Show(std::vector<std::vector<float>>& vals){

				chartControl->Series->Clear();
				System::Drawing::KnownColor myColor[] = {
					System::Drawing::KnownColor::Red,
					System::Drawing::KnownColor::Black,
					System::Drawing::KnownColor::Blue,
					System::Drawing::KnownColor::Green,
					System::Drawing::KnownColor::DarkOrange,
					System::Drawing::KnownColor::Purple
				};
				
				for (size_t i = 0, count = vals.size(); i < count; i++)
				{
					Forms::DataVisualization::Charting::Series^ series1 = gcnew Forms::DataVisualization::Charting::Series();

					series1->Name =i.ToString();
					//series1->Color = System::Drawing::Color::FromArgb(rand()%255,rand()%255,rand()%255);
					series1->Color = System::Drawing::Color::FromKnownColor( myColor[i%7]);
					series1->IsVisibleInLegend = false;
					series1->IsXValueIndexed = true;
					series1->ChartType = Forms::DataVisualization::Charting::SeriesChartType::Line;
					chartControl->Series->Add(series1);
					int pointsCount = vals[i].size();
					std::vector<float>& currV = vals[i];
					for (int j = 0; j < pointsCount; j++)
					{
						
							series1->Points->AddXY(j, currV[j]);

					}
					

				}
				
				chartControl->Invalidate();
				Show();
			}

			public: void Show(std::vector<clDPoint>& points){

				chartControl->Series->Clear();
				Forms::DataVisualization::Charting::Series^ series1 = gcnew Forms::DataVisualization::Charting::Series();


				series1->Name = "Series1";
				series1->Color = System::Drawing::Color::Green;
				series1->IsVisibleInLegend = false;
				series1->IsXValueIndexed = true;
				series1->ChartType = Forms::DataVisualization::Charting::SeriesChartType::Line;
				series1->MarkerStyle = Forms::DataVisualization::Charting::MarkerStyle::Circle;
				series1->IsXValueIndexed = false;


				chartControl->Series->Add(series1);
				int pointsCount = points.size();
				for (int i = 0; i < pointsCount; i++)
				{
					series1->Points->Add(gcnew Forms::DataVisualization::Charting::DataPoint(points[i].x,points[i].y));
				}
				chartControl->Invalidate();
				Show();
			}
			void AddPoints(std::vector<float>& vals){
				if (chartControl->Series->Count == 0)
				{
					Show(vals);
					return;
				}
				int pointsCount = chartControl->Series->FindByName("Series1")->Points->Count;
				for (size_t i = 0, count = vals.size(); i < count; i++)
				{
					chartControl->Series->FindByName("Series1")->Points->AddXY(pointsCount + i, vals[i]);

				}
				chartControl->Invalidate();

			}

			void AddPoints(std::vector<std::vector<float>>& vals){
				if (chartControl->Series->Count != vals.size())
				{
					Show(vals);
					return;
				}
				
			
				for (size_t i = 0, count = vals.size(); i < count; i++)
				{
					Forms::DataVisualization::Charting::Series^ series1 = chartControl->Series->FindByName(i.ToString());
					int pointsCount = vals[i].size();
					std::vector<float>& currV = vals[i];
					int lastindex = series1->Points->Count;
					for (int j = 0; j < pointsCount; j++)
					{

						series1->Points->AddXY(j+lastindex, currV[j]);

					}


				}

				chartControl->Invalidate();

			}
	};
}