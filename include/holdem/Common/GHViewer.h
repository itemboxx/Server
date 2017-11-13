#pragma once

namespace Forms {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GHViewer
	/// </summary>
	public ref class GHViewer : public System::Windows::Forms::Form
	{
	public:
		GHViewer(void)
		{
			InitializeComponent();
			minThresh->Text = 0.01.ToString();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GHViewer()
		{
			if (components)
			{
				delete components;
			}
			
			int a = 0;
		}
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::ListBox^  ListBoxTrade;

	private: System::Windows::Forms::ListBox^  ListBoxGHBB;

	private: System::Windows::Forms::ListBox^  ListBoxGHSB;
	private: System::Windows::Forms::Panel^  panel1;
	public: System::Windows::Forms::CheckBox^  filterCheck;
	private:
	public: System::Windows::Forms::TextBox^  minThresh;
	public: System::Windows::Forms::CheckBox^  sortByForceCheck;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  countGHBB;
	private: System::Windows::Forms::Label^  countGHSB;
	public:

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
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->ListBoxTrade = (gcnew System::Windows::Forms::ListBox());
			this->ListBoxGHBB = (gcnew System::Windows::Forms::ListBox());
			this->ListBoxGHSB = (gcnew System::Windows::Forms::ListBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->filterCheck = (gcnew System::Windows::Forms::CheckBox());
			this->minThresh = (gcnew System::Windows::Forms::TextBox());
			this->sortByForceCheck = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->countGHSB = (gcnew System::Windows::Forms::Label());
			this->countGHBB = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel1->SuspendLayout();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 3;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				33.33333F)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				33.33333F)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				33.33333F)));
			this->tableLayoutPanel1->Controls->Add(this->ListBoxTrade, 1, 0);
			this->tableLayoutPanel1->Controls->Add(this->ListBoxGHBB, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->ListBoxGHSB, 2, 0);
			this->tableLayoutPanel1->Controls->Add(this->panel1, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->label1, 1, 1);
			this->tableLayoutPanel1->Controls->Add(this->countGHSB, 2, 1);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 78)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(500, 525);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// ListBoxTrade
			// 
			this->ListBoxTrade->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ListBoxTrade->FormattingEnabled = true;
			this->ListBoxTrade->Location = System::Drawing::Point(171, 5);
			this->ListBoxTrade->Margin = System::Windows::Forms::Padding(5, 5, 5, 5);
			this->ListBoxTrade->Name = L"ListBoxTrade";
			this->ListBoxTrade->ScrollAlwaysVisible = true;
			this->ListBoxTrade->Size = System::Drawing::Size(156, 437);
			this->ListBoxTrade->TabIndex = 1;
			// 
			// ListBoxGHBB
			// 
			this->ListBoxGHBB->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ListBoxGHBB->FormattingEnabled = true;
			this->ListBoxGHBB->Location = System::Drawing::Point(5, 5);
			this->ListBoxGHBB->Margin = System::Windows::Forms::Padding(5, 5, 5, 5);
			this->ListBoxGHBB->Name = L"ListBoxGHBB";
			this->ListBoxGHBB->ScrollAlwaysVisible = true;
			this->ListBoxGHBB->Size = System::Drawing::Size(156, 437);
			this->ListBoxGHBB->TabIndex = 2;
			// 
			// ListBoxGHSB
			// 
			this->ListBoxGHSB->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ListBoxGHSB->FormattingEnabled = true;
			this->ListBoxGHSB->Location = System::Drawing::Point(337, 5);
			this->ListBoxGHSB->Margin = System::Windows::Forms::Padding(5, 5, 5, 5);
			this->ListBoxGHSB->Name = L"ListBoxGHSB";
			this->ListBoxGHSB->ScrollAlwaysVisible = true;
			this->ListBoxGHSB->Size = System::Drawing::Size(158, 437);
			this->ListBoxGHSB->TabIndex = 3;
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->countGHBB);
			this->panel1->Controls->Add(this->filterCheck);
			this->panel1->Controls->Add(this->minThresh);
			this->panel1->Controls->Add(this->sortByForceCheck);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(2, 449);
			this->panel1->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(162, 74);
			this->panel1->TabIndex = 4;
			// 
			// filterCheck
			// 
			this->filterCheck->AutoSize = true;
			this->filterCheck->Checked = true;
			this->filterCheck->CheckState = System::Windows::Forms::CheckState::Checked;
			this->filterCheck->Location = System::Drawing::Point(12, 20);
			this->filterCheck->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->filterCheck->Name = L"filterCheck";
			this->filterCheck->Size = System::Drawing::Size(92, 17);
			this->filterCheck->TabIndex = 3;
			this->filterCheck->Text = L"фильтровать";
			this->filterCheck->UseVisualStyleBackColor = true;
			this->filterCheck->CheckedChanged += gcnew System::EventHandler(this, &GHViewer::filterCheck_CheckedChanged);
			// 
			// minThresh
			// 
			this->minThresh->Location = System::Drawing::Point(102, 20);
			this->minThresh->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->minThresh->Name = L"minThresh";
			this->minThresh->Size = System::Drawing::Size(52, 20);
			this->minThresh->TabIndex = 5;
			this->minThresh->Text = L"0,01";
			this->minThresh->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GHViewer::minThresh_KeyPress);
			// 
			// sortByForceCheck
			// 
			this->sortByForceCheck->AutoSize = true;
			this->sortByForceCheck->Checked = true;
			this->sortByForceCheck->CheckState = System::Windows::Forms::CheckState::Checked;
			this->sortByForceCheck->Location = System::Drawing::Point(12, 40);
			this->sortByForceCheck->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->sortByForceCheck->Name = L"sortByForceCheck";
			this->sortByForceCheck->Size = System::Drawing::Size(132, 17);
			this->sortByForceCheck->TabIndex = 4;
			this->sortByForceCheck->Text = L"сортировать по силе";
			this->sortByForceCheck->UseVisualStyleBackColor = true;
			this->sortByForceCheck->CheckedChanged += gcnew System::EventHandler(this, &GHViewer::sortByForceCheck_CheckedChanged);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(168, 447);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(162, 78);
			this->label1->TabIndex = 5;
			this->label1->Text = L"label1";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// countGHSB
			// 
			this->countGHSB->AutoSize = true;
			this->countGHSB->Location = System::Drawing::Point(335, 447);
			this->countGHSB->Name = L"countGHSB";
			this->countGHSB->Size = System::Drawing::Size(64, 13);
			this->countGHSB->TabIndex = 6;
			this->countGHSB->Text = L"countGHSB";
			// 
			// countGHBB
			// 
			this->countGHBB->AutoSize = true;
			this->countGHBB->Location = System::Drawing::Point(10, 5);
			this->countGHBB->Name = L"countGHBB";
			this->countGHBB->Size = System::Drawing::Size(64, 13);
			this->countGHBB->TabIndex = 7;
			this->countGHBB->Text = L"countGHBB";
			// 
			// GHViewer
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(500, 525);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->Name = L"GHViewer";
			this->Text = L"GHViewer";
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

#include "C:\home\holdem\Common\clRoot.h"
		clRoot *_root;
		//cli::array<clGroupHandsAll*>^ _gh;
	public: void	ViewGH(clRoot *root)
	{
		//_gh = gcnew cli::array<clGroupHandsAll*>(2);
		_root = root;
		//_gh[0] = &root->_gh[0];
		//_gh[1] = &root->_gh[1];
		
		this->countGHBB->Text =  root->CnHand(0).ToString() + " hands";
		this->countGHSB->Text = root->CnHand(1).ToString() + " hands";
		ViewGH();

	}
			//---------------------------------------------------------------------------
	public: void ViewGH()
	{
		int arr[ALL_CN_HAND];
		arr[0] = 0;
//		if (sortByForceCheck->Checked)
//			_root->StraightHands(arr);
//		else
			for (int i = 1; i < ALL_CN_HAND; i++)
				arr[i] = i;
		double minTresh = Double::Parse(minThresh->Text);

		clHandsGroupEx &gh = _root->_gh[0];
		ListBoxGHBB->Items->Clear();
		int dim = gh.Dimension();
		for (int i = 0; i < dim; i++)
		{
			int nb = arr[i];
			if (filterCheck->Checked)
				if (gh[nb] < minTresh)
					continue;
			tpHand hand; hand.SetHandAll(nb);
			char str[8];
			//String^ as = gcnew String(((clAnsiString)hand.NameHandAll(str) + "  " + WSDoubleToAS(gh[nb], 6) + (clAnsiString)"    " + WSDoubleToAS(ev[0][nb], 6) + (clAnsiString)"    " + WSDoubleToAS(200 * (evA[0][nb] - 0.5), 6) + (clAnsiString)"    " + WSDoubleToAS(weight[0][nb], 6)).c_str());
			clAnsiString as0 = (clAnsiString)hand.NameHandAll(str);
			String^ as = gcnew String((as0 + "  " + WSDoubleToAS(gh[nb], 6)).c_str());
			ListBoxGHBB->Items->Add(as);
		}

		clHandsGroupEx &ghSB = _root->_gh[1];
		ListBoxGHSB->Items->Clear();
		for (int i = 0; i < dim; i++)
		{
			int nb = arr[i];
			if (filterCheck->Checked)
				if (ghSB[nb] < minTresh)
					continue;
			tpHand hand; hand.SetHandAll(nb);
			char str[8];
			clAnsiString as0 = (clAnsiString)hand.NameHandAll(str);
			String^ as = gcnew String((as0 + "  " + WSDoubleToAS(ghSB[nb], 6)).c_str());
			ListBoxGHSB->Items->Add(as);
		}
		ListBoxTrade->Items->Clear();
		for (int i = 0; i < _root->_sit.CnAct(); i++)
		{
			tpAction *act = _root->_sit.GetAct(i);
			clAnsiString as = HistActToString(*act);
			if (act->_dis._act == ACT_DELFLOP)
				as += (clAnsiString)" " + _root->_sit.CardsTable()[0].Name() + _root->_sit.CardsTable()[1].Name() + _root->_sit.CardsTable()[2].Name();
			else if (act->_dis._act == ACT_DELTURN)
				as += (clAnsiString)" " + _root->_sit.CardsTable()[3].Name();
			else if (act->_dis._act == ACT_DELRIVER)
				as += (clAnsiString)" " + _root->_sit.CardsTable()[4].Name();
			ListBoxTrade->Items->Add(gcnew String(as.c_str()));
		}
		label1->Text =gcnew String( WSDoubleToAS((double)_root->_sit.PlayerMoney(0) / _root->_sit.TotalPot(), 6).c_str());
		Show();
	}





	private: System::Void filterCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		ViewGH();
	}
private: System::Void sortByForceCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	ViewGH();
}
private: System::Void minThresh_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
	if (e->KeyChar == 13){
		double tmp;
		if (!Double::TryParse(minThresh->Text, tmp)){

			minThresh->Text = 0.01.ToString();

		}
		filterCheck->Checked = true;
		ViewGH();

	}
}
};
}
