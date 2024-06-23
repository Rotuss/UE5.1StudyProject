using UnrealBuildTool;

public class StudyProjectSettings : ModuleRules
{
    public StudyProjectSettings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Public�� �ٸ� ���� ���� �ܺο����� �˾ƾ��ϴ� �͵�
        PublicDependencyModuleNames.AddRange(new string[] 
        {
            // Initial Modules
            "Core", "CoreUObject", "Engine", "InputCore",
        });

        // Private�� cpp������ ����� �͵�
        PrivateDependencyModuleNames.AddRange(new string[] { });

    }
}