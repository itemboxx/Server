#pragma once

namespace Forms {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for NodeViewer
	/// </summary>
	public ref class NodeViewer : public System::Windows::Forms::Form
	{
	public:
		NodeViewer(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			 _gh = gcnew cli::array<clGroupHandsAll*>(2);
			_gh[0] = new clGroupHandsAll();
			_gh[1] = new clGroupHandsAll();
			minThresh->Text = 0.01.ToString();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~NodeViewer()
		{
			delete(_gh[0]);
			delete(_gh[1]);
			if (components)
			{
				delete components;
			}
					}
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	public: System::Windows::Forms::DataGridView^  dataGridView;
	private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel1;
	public: System::Windows::Forms::CheckBox^  filterCheck;
	private:


	public: System::Windows::Forms::CheckBox^  sortByForceCheck;
	public: System::Windows::Forms::TextBox^  minThresh;
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
			this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->flowLayoutPanel1 = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->filterCheck = (gcnew System::Windows::Forms::CheckBox());
			this->minThresh = (gcnew System::Windows::Forms::TextBox());
			this->sortByForceCheck = (gcnew System::Windows::Forms::CheckBox());
			this->tableLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->BeginInit();
			this->flowLayoutPanel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel1->Controls->Add(this->dataGridView, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->flowLayoutPanel1, 0, 1);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 100)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(648, 462);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// dataGridView
			// 
			this->dataGridView->AllowUserToAddRows = false;
			this->dataGridView->AllowUserToDeleteRows = false;
			this->dataGridView->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
			this->dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->tableLayoutPanel1->SetColumnSpan(this->dataGridView, 2);
			this->dataGridView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dataGridView->Location = System::Drawing::Point(3, 3);
			this->dataGridView->Name = L"dataGridView";
			this->dataGridView->RowTemplate->Height = 33;
			this->dataGridView->Size = System::Drawing::Size(642, 356);
			this->dataGridView->TabIndex = 0;
			// 
			// flowLayoutPanel1
			// 
			this->flowLayoutPanel1->Controls->Add(this->filterCheck);
			this->flowLayoutPanel1->Controls->Add(this->minThresh);
			this->flowLayoutPanel1->Controls->Add(this->sortByForceCheck);
			this->flowLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->flowLayoutPanel1->Location = System::Drawing::Point(3, 365);
			this->flowLayoutPanel1->Name = L"flowLayoutPanel1";
			this->flowLayoutPanel1->Size = System::Drawing::Size(318, 94);
			this->flowLayoutPanel1->TabIndex = 1;
			// 
			// filterCheck
			// 
			this->filterCheck->AutoSize = true;
			this->filterCheck->Checked = true;
			this->filterCheck->CheckState = System::Windows::Forms::CheckState::Checked;
			this->filterCheck->Location = System::Drawing::Point(3, 3);
			this->filterCheck->Name = L"filterCheck";
			this->filterCheck->Size = System::Drawing::Size(174, 29);
			this->filterCheck->TabIndex = 0;
			this->filterCheck->Text = L"фильтровать";
			this->filterCheck->UseVisualStyleBackColor = true;
			this->filterCheck->CheckStateChanged += gcnew System::EventHandler(this, &NodeViewer::filterCheck_CheckStateChanged);
			// 
			// minThresh
			// 
			this->minThresh->Location = System::Drawing::Point(183, 3);
			this->minThresh->Name = L"minThresh";
			this->minThresh->Size = System::Drawing::Size(100, 31);
			this->minThresh->TabIndex = 2;
			this->minThresh->Text = L"0.01";
			this->minThresh->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &NodeViewer::minThresh_KeyPress);
			// 
			// sortByForceCheck
			// 
			this->sortByForceCheck->AutoSize = true;
			this->sortByForceCheck->Checked = true;
			this->sortByForceCheck->CheckState = System::Windows::Forms::CheckState::Checked;
			this->sortByForceCheck->Location = System::Drawing::Point(3, 40);
			this->sortByForceCheck->Name = L"sortByForceCheck";
			this->sortByForceCheck->Size = System::Drawing::Size(252, 29);
			this->sortByForceCheck->TabIndex = 1;
			this->sortByForceCheck->Text = L"сортировать по силе";
			this->sortByForceCheck->UseVisualStyleBackColor = true;
			this->sortByForceCheck->CheckedChanged += gcnew System::EventHandler(this, &NodeViewer::sortByForceCheck_CheckedChanged);
			// 
			// NodeViewer
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(12, 25);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1000, 1000);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Name = L"NodeViewer";
			this->Text = L"NodeViewer";
			this->tableLayoutPanel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			this->flowLayoutPanel1->ResumeLayout(false);
			this->flowLayoutPanel1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
#include "C:\Home\Holdem\Strategy\AppTurnTree\Source\TurnTree.h"
		clRootStreetTree *_root;
		clStreetTree *_tree;
		clGroupHandsAll *_ghNode;
		tpEVxEVBaze *_evev;
		
		cli::array<clGroupHandsAll*,1>^ _gh;
	
		private:void 	GoToRoot(clRootStreetTree *root, clStreetTree *tree, clGroupHandsAll *gh)
		{
			if (tree->_parent == NULL)
			{
				gh[0] = root->_gh[0];
				gh[1] = root->_gh[1];
			}
			else
			{
				GoToRoot(root, (clStreetTree *)tree->_parent, gh);
				int nb = tree->Parent()->FindNbBranch(tree);
				tree->Parent()->ChangeGH(nb, gh[tree->Parent()->Indicator()]);
			}
		}


		public: void ViewInfo(clRootStreetTree* root, clStreetTree* tree)
		{
			_root = root;
			_tree = tree;
			clGroupHandsAll ghLocal[2];
			ghLocal[0] = root->_gh[0];
			ghLocal[1] = root->_gh[1];
			GoToRoot(_root, tree, ghLocal);
			ghLocal[0].Norm();
			ghLocal[1].Norm();
			*_gh[0] = ghLocal[0];
			*_gh[1] = ghLocal[1];
			ViewInfo();

		}

public: void ViewInfo()
{
	int cnB = _tree->CnBranch();
	int ind = _tree->Indicator();

	if (cnB < 2)
	{
		//Clear();
		//Show();
		return;
	}
	DataGridView% table = *dataGridView;
	table.Rows->Clear();
	table.ColumnCount = cnB + 1;
	for (int i = 0; i < cnB; i++)
	{
		table.Columns[i + 1]->HeaderText = gcnew System::String(_tree->NameForBranch(i).c_str());
	}

	int arr[ALL_CN_HAND];
	arr[0] = 0;
	/*if (sortByForceCheck->Checked)
	_root->StraightHands(arr);
	else*/
	for (int i = 1; i < ALL_CN_HAND; i++)
		//							arr[i] = _root->_index[i];
		arr[i] = i;
	double minTresh = Double::Parse(minThresh->Text);

	//table.RowCount = ALL_CN_HAND + 1;
	int nbT = 0;
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		int nb = arr[i];
		if (filterCheck->Checked)
			if ((*_gh[ind])[nb] < minTresh)
				continue;
		char str[8];
		tpHand hand; hand.SetHandAll(nb);
		table.Rows->Add();
		String^ str1 = gcnew String(((clAnsiString)nb + " " + hand.NameHandAll(str) + " " + WSDoubleToAS((*(_gh[ind]))[nb], 6)).c_str());
		table.Rows[nbT]->Cells[0]->Value =
			//gcnew String(WSDoubleToAS((*_gh[ind])[nb], 6).c_str())
			gcnew String(((clAnsiString)nb + " " + hand.NameHandAll(str) + " " + WSDoubleToAS((*(_gh[ind]))[nb], 6)).c_str())
			;
		clAnsiString tmp = WSDoubleToAS((*_gh[ind])[nb], 6);
		//TabStr->Cells[0][nbT] = (AnsiString)_root->_index[nb] + " " + hand.NameHandAll(str) + "  " + WSDoubleToAS(_gh[ind][nb], 6);
		int nbCls = _root->GetNbCls(_tree, nb);
		clCalcParam cls = _tree->AtrTree()->CalcParam(nbCls);
		tpFloat w[MAX_CN_BRANCH];
		cls.GetWeightStrat(cnB, w);
		for (int k = 0; k < cnB; k++) {
			String^ str = gcnew String((WSDoubleToAS(w[k], 6) + " ev=" + WSDoubleToAS(cls.AbsEV(k), 6)).c_str());
			table.Rows[nbT]->Cells[k + 1]->Value = gcnew String((WSDoubleToAS(w[k], 6) + " ev=" + WSDoubleToAS(cls.AbsEV(k), 6)).c_str());
			double tmp = cls.AbsEV(k);
		}
		nbT++;
	}

	Show();

}

		public: void ViewInfoNodeEv(clRootStreetTree* root, clStreetTree* tree, clGroupHandsAll *gh, tpEVxEVBaze *ev)
		{
			_root = root;
			_tree = tree;
			_ghNode=gh;
			_evev=ev;
			ViewInfoNodeEv();

		}

public: void ViewInfoNodeEv()
{
	int ind = 0;
	int opp = (ind == 0) ? 1 : 0;
	DataGridView% table = *dataGridView;
	table.Rows->Clear();
	//tpEVxEVBaze matr;
	double min = Double::Parse(minThresh->Text);
	int cnB = 0;
	clGroupHandsAll &ghInd = _ghNode[ind];
	clGroupHandsAll &ghOpp = _ghNode[opp];
	const int maxCnHand = 20;
	int nbOpp[maxCnHand];
	for (int i = 0; i < _root->_cnHand[opp]; i++)
	{
		int nb = _root->_nbHand[opp][i];
		if (ghOpp[nb] > min)
		{
			nbOpp[cnB++] = i;
			if (cnB == maxCnHand)
				break;
		}
	}
	table.ColumnCount = cnB + 1;

	char str[16];
	for (int i = 0; i < cnB; i++)
	{
		int nb = _root->_nbHand[opp][nbOpp[i]];
		HandName(nb, str);
		table.Columns[i+1]->HeaderText = gcnew System::String(str);
	}

	int nbT = 0;
	for (int i = 0; i < _root->_cnHand[ind]; i++)
	{
		int nb = _root->_nbHand[ind][i];
		if (ghInd[nb] > min)
		{
			char str[16];
			table.Rows->Add();
			clAnsiString as = (clAnsiString)nb + " " + (clAnsiString)HandName(nb, str) + " " + WSDoubleToAS(ghInd[nb], 6);
			table.Rows[nbT]->Cells[0]->Value = gcnew String(as.c_str());
			for (int k = 0; k < cnB; k++) 
			{
				as = WSDoubleToAS((*_evev)[i][nbOpp[k]], 6);
				table.Rows[nbT]->Cells[k + 1]->Value = gcnew String(as.c_str());
			}
			nbT++;
		}
	}

	Show();
}

	private: System::Void filterCheck_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) {
		ViewInfo();
	}
private: System::Void sortByForceCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	ViewInfo();
}
private: System::Void minThresh_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
	if (e->KeyChar==13){
		double tmp;
		if (!Double::TryParse(minThresh->Text,tmp)){
	
			minThresh->Text = "0.01";
		}
		filterCheck->Checked = true;
		ViewInfo();
		
	}
}
};
}
