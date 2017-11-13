#pragma once
#include <vector>
#include <string>
#include <msclr\marshal_cppstd.h>
	namespace Forms {

		using namespace msclr::interop;
		using namespace System;
		using namespace System::ComponentModel;
		using namespace System::Collections;
		using namespace System::Windows::Forms;
		using namespace System::Data;
		using namespace System::Drawing;
		using namespace Generic;
		

		/// <summary>
		/// Summary for PropsdertyEditorDialog
		/// Showing labels+editboxes 
		/// </summary>
		public ref class PropertyEditorDialog : public System::Windows::Forms::Form
		{
		public:
			PropertyEditorDialog(void)
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
			~PropertyEditorDialog()
			{
				if (components)
				{
					delete components;
				}
			}
		private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
		protected:

		private: System::Windows::Forms::Button^  okButton;
		private: System::Windows::Forms::Button^  cancelButton;
		private: System::Windows::Forms::Panel^  settingsPanel;
		private: System::Windows::Forms::TableLayoutPanel^  tableOfControls;







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
				this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
				this->cancelButton = (gcnew System::Windows::Forms::Button());
				this->okButton = (gcnew System::Windows::Forms::Button());
				this->settingsPanel = (gcnew System::Windows::Forms::Panel());
				this->tableOfControls = (gcnew System::Windows::Forms::TableLayoutPanel());
				this->tableLayoutPanel1->SuspendLayout();
				this->settingsPanel->SuspendLayout();
				this->SuspendLayout();
				// 
				// tableLayoutPanel1
				// 
				this->tableLayoutPanel1->ColumnCount = 2;
				this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					50)));
				this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					50)));
				this->tableLayoutPanel1->Controls->Add(this->cancelButton, 1, 1);
				this->tableLayoutPanel1->Controls->Add(this->okButton, 0, 1);
				this->tableLayoutPanel1->Controls->Add(this->settingsPanel, 0, 0);
				this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
				this->tableLayoutPanel1->Location = System::Drawing::Point(10, 10);
				this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(10);
				this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
				this->tableLayoutPanel1->RowCount = 2;
				this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
				this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
				this->tableLayoutPanel1->Size = System::Drawing::Size(304, 155);
				this->tableLayoutPanel1->TabIndex = 0;
				// 
				// cancelButton
				// 
				this->cancelButton->Dock = System::Windows::Forms::DockStyle::Fill;
				this->cancelButton->Location = System::Drawing::Point(157, 97);
				this->cancelButton->Margin = System::Windows::Forms::Padding(5);
				this->cancelButton->Name = L"cancelButton";
				this->cancelButton->Size = System::Drawing::Size(142, 53);
				this->cancelButton->TabIndex = 1;
				this->cancelButton->Text = L"Отмена";
				this->cancelButton->UseVisualStyleBackColor = true;
				this->cancelButton->Click += gcnew System::EventHandler(this, &PropertyEditorDialog::cancelButton_Click);
				// 
				// okButton
				// 
				this->okButton->Dock = System::Windows::Forms::DockStyle::Fill;
				this->okButton->Location = System::Drawing::Point(5, 97);
				this->okButton->Margin = System::Windows::Forms::Padding(5);
				this->okButton->Name = L"okButton";
				this->okButton->Size = System::Drawing::Size(142, 53);
				this->okButton->TabIndex = 0;
				this->okButton->Text = L"OK";
				this->okButton->UseVisualStyleBackColor = true;
				this->okButton->Click += gcnew System::EventHandler(this, &PropertyEditorDialog::okButton_Click);
				// 
				// settingsPanel
				// 
				this->tableLayoutPanel1->SetColumnSpan(this->settingsPanel, 2);
				this->settingsPanel->Controls->Add(this->tableOfControls);
				this->settingsPanel->Dock = System::Windows::Forms::DockStyle::Fill;
				this->settingsPanel->Location = System::Drawing::Point(3, 3);
				this->settingsPanel->Name = L"settingsPanel";
				this->settingsPanel->Size = System::Drawing::Size(298, 86);
				this->settingsPanel->TabIndex = 1;
				// 
				// tableOfControls
				// 
				this->tableOfControls->ColumnCount = 2;
				this->tableOfControls->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					50)));
				this->tableOfControls->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					50)));
				this->tableOfControls->Dock = System::Windows::Forms::DockStyle::Fill;
				this->tableOfControls->Location = System::Drawing::Point(0, 0);
				this->tableOfControls->Name = L"tableOfControls";
				this->tableOfControls->RowCount = 1;
				this->tableOfControls->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
				this->tableOfControls->Size = System::Drawing::Size(298, 86);
				this->tableOfControls->TabIndex = 0;
				// 
				// PropertyEditorDialog
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(12, 25);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->AutoSize = true;
				this->ClientSize = System::Drawing::Size(324, 175);
				this->Controls->Add(this->tableLayoutPanel1);
				this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
				this->Name = L"PropertyEditorDialog";
				this->Padding = System::Windows::Forms::Padding(10);
				this->Text = L"Settings";
				this->Shown += gcnew System::EventHandler(this, &PropertyEditorDialog::PropertyEditorDialog_Shown);
				this->tableLayoutPanel1->ResumeLayout(false);
				this->settingsPanel->ResumeLayout(false);
				this->ResumeLayout(false);

			}
#pragma endregion
		private: System::Void PropertyEditorDialog_Shown(System::Object^  sender, System::EventArgs^  e) {

		}

		private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e) {
			int count = valueEdits->Length;
			for (size_t i = 0; i < count; i++)
			{
				valuesToSet->at(i) = marshal_as<std::string>(valueEdits[i]->Text);
			}

			this->DialogResult = Forms::DialogResult::OK;
			this->Close();


		}
		private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {

			this->DialogResult = Forms::DialogResult::Cancel;
			this->Close();
		}
		private: cli::array<Windows::Forms::Label^,1>^   valueLabels;
		private: cli::array<Windows::Forms::TextBox^,1>^ valueEdits;
		private: std::vector<std::string>* valuesToSet;
		public: Forms::DialogResult ShowDialog(std::vector<std::string> labels, std::vector<std::string>* values){
			int controlsCount = labels.size();
			valuesToSet = values;
			valueLabels = gcnew	cli::array<Label^>(controlsCount);
			valueEdits = gcnew cli::array<TextBox^>(controlsCount);
			tableOfControls->RowCount = controlsCount;

			Height = 170 + 50 * controlsCount;
			for (size_t i = 0; i < controlsCount; i++)
			{
				valueLabels[i] = gcnew Label();
				valueEdits[i] = gcnew TextBox();
				Label% l = *valueLabels[i];
				TextBox% t = *valueEdits[i];
				l.Text = gcnew String(labels[i].c_str());
				t.Text = gcnew String(values->at(i).c_str());
				tableOfControls->Controls->Add(%l, 0, i);
				tableOfControls->Controls->Add(%t, 1, i);
				tableOfControls->RowStyles->Add(gcnew RowStyle());
				tableOfControls->RowStyles[i]->SizeType = SizeType::Percent;
				tableOfControls->RowStyles[i]->Height = 100.0 / controlsCount;
				l.Dock = DockStyle::None;
				l.Anchor = AnchorStyles::Right | AnchorStyles::Left;
				t.Dock = DockStyle::None;
				t.Anchor = AnchorStyles::Right | AnchorStyles::Left;
			}
			
			//label1->Text = gcnew String( labels[0].c_str());

			return ShowDialog();
		}
		};

	}
