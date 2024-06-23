using UnrealBuildTool;

public class StudyProjectSettings : ModuleRules
{
    public StudyProjectSettings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Public은 다른 모듈과 같은 외부에서도 알아야하는 것들
        PublicDependencyModuleNames.AddRange(new string[] 
        {
            // Initial Modules
            "Core", "CoreUObject", "Engine", "InputCore",
        });

        // Private은 cpp에서만 사용할 것들
        PrivateDependencyModuleNames.AddRange(new string[] { });

    }
}