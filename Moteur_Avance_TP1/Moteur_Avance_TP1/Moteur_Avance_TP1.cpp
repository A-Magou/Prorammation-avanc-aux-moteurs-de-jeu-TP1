
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


string BUILD_PATH = "./Engine/Build/BatchFiles/Build.bat";
string UNREAL_EDITOR_CMD_PATH = "./Engine/Binaries/Win64/UnrealEditor-Cmd.exe";
string UAT_PATH = "./Engine/Build/BatchFiles/RunUAT.bat";

void WrongCommand()
{
    cout << "Usage: Moteur_Avance_TP1 [CHEMIN_DU_PROJET] [COMMANDE] [CHEMIN_DUPACKAGE (if applicable)]" << endl;
    cout << "available [COMMANDE]: " << endl
        << "show-infos" << endl
        << "build" << endl
        << "package" << endl
    ;
}

void ShowInfos(string uprojectPath)
{
    cout << "Trying to access to .uproject file." << endl << endl;
    
    //Open .uproject file
    ifstream file (uprojectPath);
    if (!file.is_open())
    {
        cout << "Error opening file " << uprojectPath << endl;
        return;
    }

    //Get file data
    json uprojectData;
    uprojectData = json::parse(file);

    //Get file name and display it
    string uprojectName = fs::path(uprojectPath).stem().string();
    cout << "Engine name: " << uprojectName << endl;

    //Get engine version and display it
    if (uprojectData.contains("EngineAssociation"))
    {
        string engineVersion = uprojectData["EngineAssociation"];

        if (!engineVersion.empty() && engineVersion.front() == '{' && engineVersion.back() == '}')
        {
            cout << "Engine from source" << endl;
        }
        else
        {
            cout << "Engine Version: " << engineVersion << endl;
        }
    }
    else
    {
        cout << "EngineVersion not found" << endl;
    }

    //Get all plugins and display them
    if (uprojectData.contains("Plugins"))
    {
        cout << "Plugins used: " << endl;
        for (const auto& plugin : uprojectData["Plugins"])
        {
            if (plugin.contains("Name") && plugin.contains("Enabled"))
            {
                cout << "  - " << plugin["Name"].get<string>()
                    << " (Enabled: " << (plugin["Enabled"].get<bool>() ? "Yes" : "No") << ")" << endl;
            }
        }
    }
    else
    {
        cout << "Plugins not found" << endl;
    }
}

void BuildProject(string uprojectPath)
{
    string projectName = fs::path(uprojectPath).stem().string();
    string target = "Win64";
    string optimization = "Development";

    string buildCommandLine = BUILD_PATH + " " 
                            + projectName + " "
                            + target + " "
                            + optimization + " \""
                            + uprojectPath + "\" -waitmutex"
    ;

    cout << "Executing build command: " << endl << buildCommandLine << endl;

    int ret = system(buildCommandLine.c_str());
    if (ret != 0)
    {
        cout << "Build command execution failed with exit code: " << ret << endl;
    }
}

void PackageProject(string uprojectPath, string packagePath)
{
    
    string packageCommandLine =
        UAT_PATH + " "
        + "-ScriptsForProject=\"" + uprojectPath + "\" BuildCookRun "
        + "-project=\"" + uprojectPath + "\" "
        + "-noP4 "
        + "-clientconfig=Development "
        + "-nocompileeditor "
        + "-unrealexe=\"" + UNREAL_EDITOR_CMD_PATH + "\" "
        + "-utf8output "
        + "-platform=Win64 "
        + "-build "
        + "-cook "
        + "-map=ThirdPersonMap+ThirdPersonMap "
        + "-CookCultures=en "
        + "-unversionedcookedcontent "
        + "-stage "
        + "-package "
        + "-addcmdline=\"-SessionId=1F2A65104A0E318E7F6C9EAF5976D0D4 -SessionOwner='Tianl' -SessionName='Un_Nom_Interessant'\" "
        + "-packagepath=\"" + packagePath + "\""
    ;

    cout << "Executing UAT Package Command: " << endl << packageCommandLine << endl << endl << endl;

    int ret = system(packageCommandLine.c_str());
    if (ret != 0)
    {
        cout << "Package command execution failed with exit code: " << ret << endl;
    }
}

int main(int argc, char *argv[])
{
    
    if (argc < 3)
    {
        WrongCommand();
        return 1;
    }

    string uprojectPath = argv[1];
    string command = argv[2];

    if (command == "show-infos")
    {
        ShowInfos(uprojectPath);
    }
    else if (command == "build")
    {
        BuildProject(uprojectPath);
    }
    else if (command == "package")
    {
        if (argc < 4)
        {
            cout << "Error: Missing package path for packaging." << endl;
            return 1;
        }
        string packagePath = argv[3];
        PackageProject(uprojectPath, packagePath);
    }
    else
    {
        WrongCommand();
        return 1;
    }
    
    /*
    cout << "Choose command: " << endl
        << "1: show-infos" << endl
        << "2: build" << endl
        << "3: package" << endl;

    string command;
    cin >> command;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string uprojectPath = "";
    cout << "pls enter path with file.uprojct" << endl;
    getline(cin, uprojectPath);
    
    if (command == "1")
    {
        ShowInfos(uprojectPath);
    }
    else if (command == "2")
    {
        BuildProject(uprojectPath);
    }
    else if (command == "3")
    {
        string packagePath = "";
        cout << "pls enter path of your package" << endl;
        getline(cin, packagePath);

        PackageProject(uprojectPath, packagePath);
    }
    */
    
    return 0;
}

