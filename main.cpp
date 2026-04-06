#include <wx/wx.h>
#include <wx/artprov.h>
#include <windows.h>


class Frame : public wxFrame
{
    private:
    wxPanel *pnl;
    wxCheckBox *chkRemoto;
    wxButton *btnExecutar, *btnDestinoFDB;
    wxRadioBox *radioFirebirdVers;
    wxTextCtrl *campoIp, *campoPorta, *campoBD, *campoUsuario, *campoSenha, *campoNucleos, *campoDestino;
    long maximoDeNucleos;

    public:
        void on_btn1_clicked(wxCommandEvent& evt){

        }

        Frame() : wxFrame(nullptr,
                        wxID_ANY, 
                        "Backup", 
                        wxDefaultPosition, 
                        wxSize(700, 400), 
                        wxDEFAULT_FRAME_STYLE 
                        & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
        {
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            maximoDeNucleos = sysInfo.dwNumberOfProcessors;
            wxIcon icone = wxArtProvider::GetIcon(wxART_FOLDER);
            this->SetIcon(icone);

            wxArrayString opcoes;
            opcoes.Add("Firebird 3");
            opcoes.Add("Firebird 4");
            opcoes.Add("Firebird 5");
            

            pnl = new wxPanel(this);
            btnExecutar = new wxButton(pnl, wxID_ANY, "Executar", wxPoint(20, 300), wxSize(-1, -1));
            
            new wxStaticText(pnl, wxID_ANY, "IP", wxPoint(20, 20), wxSize(-1, -1));
            campoIp = new wxTextCtrl(pnl, wxID_ANY, "127.0.0.1", wxPoint(20, 40), wxSize(200, -1));
            
            new wxStaticText(pnl, wxID_ANY, "Porta", wxPoint(240, 20), wxSize(-1, -1));
            campoPorta = new wxTextCtrl(pnl, wxID_ANY, "3050",wxPoint(240, 40), wxSize(60, -1));
            
            new wxStaticText(pnl, wxID_ANY, "Caminho do banco (.fdb)", wxPoint(20, 70), wxSize(-1, -1));
            campoBD = new wxTextCtrl(pnl, wxID_ANY, "", wxPoint(20, 90), wxSize(300, -1));

            btnDestinoFDB = new wxButton(pnl, wxID_ANY, "Selecionar", wxPoint(20, 130), wxSize(-1, -1));

            new wxStaticText(pnl, wxID_ANY, "Caminho do backup do banco (.fbk)", wxPoint(330, 70), wxSize(-1, -1));
            campoDestino = new wxTextCtrl(pnl, wxID_ANY, "", wxPoint(330, 90), wxSize(300, -1));
            
            new wxStaticText(pnl, wxID_ANY, wxT("Usuário"), wxPoint(20, 190), wxSize(-1, -1));
            campoUsuario = new wxTextCtrl(pnl, wxID_ANY, "SYSDBA", wxPoint(20, 210), wxSize(200, -1));
            
            new wxStaticText(pnl, wxID_ANY, "Senha", wxPoint(20, 250), wxSize(-1, -1));
            campoSenha = new wxTextCtrl(pnl, wxID_ANY, "", wxPoint(20, 270), wxSize(200, -1), wxTE_PASSWORD);

            new wxStaticText(pnl, wxID_ANY, wxT("Núcleos"), wxPoint(420, 20), wxSize(-1, -1));
            campoNucleos = new wxTextCtrl(pnl, wxID_ANY, "1", wxPoint(420, 40), wxSize(60, -1));

            chkRemoto = new wxCheckBox(pnl, wxID_ANY, "Backup remoto (backup será criado no servidor)", wxPoint(330, 240));

            radioFirebirdVers = new wxRadioBox(pnl, wxID_ANY, wxT("Versão do Firebird"),
                            wxPoint(330, 130), wxSize(-1, -1), opcoes, 1, wxRA_SPECIFY_COLS);
            radioFirebirdVers->SetSelection(2);
            
            campoBD->Bind(wxEVT_KILL_FOCUS, [this](wxFocusEvent& evt){
                if(campoBD->GetValue().Lower().EndsWith("fdb")){
                    wxString nomeDB = campoBD->GetValue();
                    nomeDB = nomeDB.Left(nomeDB.Len() - 3) + "fbk";
                    campoDestino->SetValue(nomeDB);
                }
                evt.Skip();
            });
            radioFirebirdVers->Bind(wxEVT_RADIOBOX, [this](wxCommandEvent& evt){
                int versao = radioFirebirdVers->GetSelection();
                switch(versao){
                    case 0: {
                        wxMessageBox(wxT("Na versão 3 do Firebird, não é suportado\n"
                                "parâmetros de compressão e multi-threading"), "Alerta", 
                                wxOK | wxICON_WARNING, this);
                        campoNucleos->Enable(false);
                    } break;
                    case 1: {
                        wxMessageBox(wxT("Na versão 4 do Firebird, não é suportado\n"
                                "parâmetro de multi-threading"), "Alerta",
                                wxOK | wxICON_WARNING, this);
                        campoNucleos->Enable(false);
                    } break;
                    case 2: {
                        campoNucleos->Enable(true);
                    } break;
                    default: return; break;
                }
            });

            btnDestinoFDB->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt){
                wxFileDialog dialogo(
                    this,
                    L"Selecione o banco .FDB",
                    "",
                    ".fdb",
                    "Firebird Backup (*.fdb) |*.fdb",
                    wxFD_OPEN
                );

                if(dialogo.ShowModal() == wxID_OK){
                    campoBD->SetValue(dialogo.GetPath());
                }
                if(campoBD->GetValue().Lower().EndsWith("fdb")){
                    wxString nomeDB = campoBD->GetValue();
                    nomeDB = nomeDB.Left(nomeDB.Len() - 3) + "fbk";
                    campoDestino->SetValue(nomeDB);
                }
            });

            btnExecutar->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt){
                if(campoIp->GetValue().IsEmpty() ||
                    campoBD->GetValue().IsEmpty() ||
                    campoDestino->GetValue().IsEmpty() ||
                    campoUsuario->GetValue().IsEmpty() ||
                    campoSenha->GetValue().IsEmpty())
                {
                    wxMessageBox(wxT("Preencha todos os campos obrigatórios."), 
                                wxT("Atenção"), wxOK | wxICON_WARNING, this);
                    return;
                }
                long valorNucleos;
                wxString comando = "";
                if(campoNucleos->GetValue().ToLong(&valorNucleos)){
                    if(valorNucleos >= maximoDeNucleos){
                        wxMessageBox(wxT("Quantidade de processadores lógicos maior "
                            "ou igual ao existente.\nReduzindo para um valor seguro"),
                        "Alerta", wxOK | wxICON_WARNING);
                        campoNucleos->SetValue(wxString::Format("%d", maximoDeNucleos - 1));
                        return;
                    }
                } 
                else{
                    if(radioFirebirdVers->GetSelection() == 2){
                        wxMessageBox(wxT("Informe um número válido de núcleos."), 
                            wxT("Atenção"), wxOK | wxICON_WARNING, this);
                        campoNucleos->SetValue("1");
                        return;
                    }
                }

                if(chkRemoto->GetValue()){
                    switch(radioFirebirdVers->GetSelection()){
                        case 0:
                            comando = wxString::Format(
                                "\"C:/Program Files/Firebird/Firebird_3_0/gbak.exe\""
                                " -b -se %s/%s:service_mgr "
                                "-user %s -password %s "
                                " \"%s\""
                                " \"%s\"",
                                campoIp->GetValue(),
                                campoPorta->GetValue(),
                                campoUsuario->GetValue(),
                                campoSenha->GetValue(),
                                campoBD->GetValue(),
                                campoDestino->GetValue()
                            );
                            break;
                        case 1:
                            comando = wxString::Format(
                                "\"C:/Program Files/Firebird/Firebird_4_0/gbak.exe\""
                                " -b -zip -se %s/%s:service_mgr "
                                "-user %s -password %s "
                                " \"%s\""
                                " \"%s\"",
                                campoIp->GetValue(),
                                campoPorta->GetValue(),
                                campoUsuario->GetValue(),
                                campoSenha->GetValue(),
                                campoBD->GetValue(),
                                campoDestino->GetValue()
                            );
                            break;
                        case 2:
                            comando = wxString::Format(
                                "\"C:/Program Files/Firebird/Firebird_5_0/gbak.exe\""
                                " -b -zip -par %s -se %s/%s:service_mgr "
                                "-user %s -password %s "
                                " \"%s\""
                                " \"%s\"",
                                campoNucleos->GetValue(),
                                campoIp->GetValue(),
                                campoPorta->GetValue(),
                                campoUsuario->GetValue(),
                                campoSenha->GetValue(),
                                campoBD->GetValue(),
                                campoDestino->GetValue()
                            );
                            break;
                        default: break;
                    }
                }
                else
                {
                    switch(radioFirebirdVers->GetSelection()){
                        case 0:
                            comando = wxString::Format(
                                        "\"C:/Program Files/Firebird/Firebird_3_0/gbak.exe\""
                                        " -b "
                                        "-user %s -password %s "
                                        " \"%s/%s:%s\""
                                        " \"%s\"",
                                        campoUsuario->GetValue(),
                                        campoSenha->GetValue(),
                                        campoIp->GetValue(),
                                        campoPorta->GetValue(),
                                        campoBD->GetValue(),
                                        campoDestino->GetValue()
                                    );
                        break;
                        case 1:
                            comando = wxString::Format(
                                        "\"C:/Program Files/Firebird/Firebird_4_0/gbak.exe\""
                                        " -b -zip "
                                        "-user %s -password %s "
                                        " \"%s/%s:%s\""
                                        " \"%s\"",
                                        campoUsuario->GetValue(),
                                        campoSenha->GetValue(),
                                        campoIp->GetValue(),
                                        campoPorta->GetValue(),
                                        campoBD->GetValue(),
                                        campoDestino->GetValue()
                                    );
                        break;
                        case 2:
                            comando = wxString::Format(
                                        "\"C:/Program Files/Firebird/Firebird_5_0/gbak.exe\""
                                        " -b -zip -par %s "
                                        "-user %s -password %s "
                                        " \"%s/%s:%s\""
                                        " \"%s\"",
                                        campoNucleos->GetValue(),
                                        campoUsuario->GetValue(),
                                        campoSenha->GetValue(),
                                        campoIp->GetValue(),
                                        campoPorta->GetValue(),
                                        campoBD->GetValue(),
                                        campoDestino->GetValue()
                                    );
                        break;
                        default: break;
                    }
                }
                
                wxArrayString saida, erro;

                long resultado = wxExecute(comando, saida, erro, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE);
                    if(resultado == 0){
                        wxMessageBox("Backup realizado com sucesso", "Backup", wxOK | wxICON_INFORMATION);
                    } else {
                        wxString msgErro = "Erro ao realizar backup.\n\n";

                        if(!erro.IsEmpty()){
                            msgErro += "Detalhes\n";
                            for(const wxString& linha : erro)
                                msgErro += linha + "\n";
                        }
                        else if(!saida.IsEmpty()){
                            msgErro += "Saída\n";
                            for(const wxString& linha : saida)
                                msgErro += linha + "\n";
                        }
                        
                        msgErro += wxString::Format(wxT("\nCódigo de erro: %ld"), resultado);

                        wxMessageBox(msgErro, "Erro", wxOK | wxICON_ERROR, this);
                    }
                });
        }
};

class app : public wxApp
{
    wxLocale locale;

public:
    virtual bool OnInit(){
        locale.Init(wxLANGUAGE_DEFAULT);
        wxConvCurrent = &wxConvUTF8;
        Frame* _frame = new Frame();
        _frame->Show(true);
        return true;
    }  
};

wxIMPLEMENT_APP(app);
