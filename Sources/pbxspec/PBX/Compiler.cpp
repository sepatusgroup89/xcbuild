// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/Compiler.h>
#include <pbxspec/PBX/CompilerSpecificationGcc.h>
#include <pbxspec/PBX/CompilerSpecificationMig.h>
#include <pbxspec/XC/CompilerSpecificationClang.h>
#include <pbxspec/Manager.h>

using pbxspec::PBX::Compiler;

Compiler::Compiler(bool isDefault) :
    Compiler(isDefault, ISA::PBXCompiler)
{
}

Compiler::Compiler(bool isDefault, std::string const &isa) :
    Tool                                     (isDefault, isa),
    _overridingProperties                    (nullptr),
    _useCPlusPlusCompilerDriverWhenBundlizing(false),
    _dashIFlagAcceptHeadermaps               (false),
    _supportsHeadermaps                      (false),
    _supportsIsysroot                        (false),
    _supportsSeparateUserHeaderPaths         (false),
    _supportsGeneratePreprocessedFile        (false),
    _supportsGenerateAssemblyFile            (false),
    _supportsAnalyzeFile                     (false),
    _supportsSerializedDiagnostics           (false),
    _supportsPredictiveCompilation           (false),
    _supportsMacOSXDeploymentTarget          (false),
    _supportsMacOSXMinVersionFlag            (false),
    _prunePrecompiledHeaderCache             (false),
    _outputAreProducts                       (false),
    _outputAreSourceFiles                    (false),
    _softError                               (false),
    _deeplyStatInputDirectories              (false),
    _dontProcessOutputs                      (false),
    _showInCompilerSelectionPopup            (false),
    _showOnlySelfDefinedProperties           (false)
{
}

Compiler::~Compiler()
{
    if (_overridingProperties != nullptr) {
        _overridingProperties->release();
    }
}

bool Compiler::
inherit(Specification::shared_ptr const &base)
{
    if (!base->isa(Compiler::Isa()))
        return false;

    return inherit(reinterpret_cast <Compiler::shared_ptr const &> (base));
}

bool Compiler::
inherit(Tool::shared_ptr const &base)
{
    if (!base->isa(Compiler::Isa()))
        return false;

    return inherit(reinterpret_cast <Compiler::shared_ptr const &> (base));
}

bool Compiler::
inherit(Compiler::shared_ptr const &b)
{
    if (!Tool::inherit(reinterpret_cast <Tool::shared_ptr const &> (b)))
        return false;

    auto base = this->base();

    _execCPlusPlusLinkerPath                  = base->execCPlusPlusLinkerPath();
    _executionDescription                     = base->executionDescription();
    _sourceFileOption                         = base->sourceFileOption();
    _outputDir                                = base->outputDir();
    _outputFileExtension                      = base->outputFileExtension();
    _commandResultsPostprocessor              = base->commandResultsPostprocessor();
    _generatedInfoPlistContentFilePath        = base->generatedInfoPlistContentFilePath();
    _dependencyInfoFile                       = base->dependencyInfoFile();
    _dependencyInfoArgs                       = base->dependencyInfoArgs();
    _languages                                = base->languages();
    _optionConditionFlavors                   = base->optionConditionFlavors();
    _patternsOfFlagsNotAffectingPrecomps      = base->patternsOfFlagsNotAffectingPrecomps();
    _messageCategoryInfoOptions               = base->messageCategoryInfoOptions();
    _synthesizeBuildRuleForBuildPhases        = base->synthesizeBuildRuleForBuildPhases();
    _inputFileGroupings                       = base->inputFileGroupings();
    _fallbackTools                            = base->fallbackTools();
    _overridingProperties                     = plist::Copy(base->overridingProperties());
    _useCPlusPlusCompilerDriverWhenBundlizing = base->useCPlusPlusCompilerDriverWhenBundlizing();
    _supportsHeadermaps                       = base->supportsHeadermaps();
    _supportsIsysroot                         = base->supportsIsysroot();
    _supportsSeparateUserHeaderPaths          = base->supportsSeparateUserHeaderPaths();
    _supportsGeneratePreprocessedFile         = base->supportsGeneratePreprocessedFile();
    _supportsGenerateAssemblyFile             = base->supportsGenerateAssemblyFile();
    _supportsAnalyzeFile                      = base->supportsAnalyzeFile();
    _supportsSerializedDiagnostics            = base->supportsSerializedDiagnostics();
    _supportsPredictiveCompilation            = base->supportsPredictiveCompilation();
    _supportsMacOSXDeploymentTarget           = base->supportsMacOSXDeploymentTarget();
    _supportsMacOSXMinVersionFlag             = base->supportsMacOSXMinVersionFlag();
    _prunePrecompiledHeaderCache              = base->prunePrecompiledHeaderCache();
    _outputAreProducts                        = base->outputAreProducts();
    _outputAreSourceFiles                     = base->outputAreSourceFiles();
    _softError                                = base->softError();
    _deeplyStatInputDirectories               = base->deeplyStatInputDirectories();
    _dontProcessOutputs                       = base->dontProcessOutputs();
    _showInCompilerSelectionPopup             = base->showInCompilerSelectionPopup();
    _showOnlySelfDefinedProperties            = base->showOnlySelfDefinedProperties();

    return true;
}

Compiler::shared_ptr Compiler::
Parse(plist::Dictionary const *dict)
{
    Compiler::shared_ptr result;

    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new Compiler(true));
    } else if (C->value() == CompilerSpecificationGcc::Isa()) {
        result.reset(new CompilerSpecificationGcc(true));
    } else if (C->value() == CompilerSpecificationMig::Isa()) {
        result.reset(new CompilerSpecificationMig(true));
    } else if (C->value() == XC::CompilerSpecificationClang::Isa()) {
        result.reset(new XC::CompilerSpecificationClang(true));
    } else {
        fprintf(stderr, "warning: compiler class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new Compiler(true));
    }

    if (!result->parse(dict))
        return nullptr;

    return result;
}

bool Compiler::
parse(plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "Compiler",
        // Specification
        plist::MakeKey <plist::String> ("Class"),
        plist::MakeKey <plist::String> ("Type"),
        plist::MakeKey <plist::String> ("Identifier"),
        plist::MakeKey <plist::String> ("BasedOn"),
        plist::MakeKey <plist::String> ("Name"),
        plist::MakeKey <plist::String> ("Description"),
        plist::MakeKey <plist::String> ("Vendor"),
        plist::MakeKey <plist::String> ("Version"),
        // Tool
        plist::MakeKey <plist::String> ("ExecPath"),
        plist::MakeKey <plist::String> ("ExecDescription"),
        plist::MakeKey <plist::String> ("ProgressDescription"),
        plist::MakeKey <plist::String> ("CommandLine"),
        plist::MakeKey <plist::String> ("CommandInvocationClass"),
        plist::MakeKey <plist::Object> ("RuleName"),
        plist::MakeKey <plist::String> ("RuleFormat"),
        plist::MakeKey <plist::String> ("AdditionalInputFiles"),
        plist::MakeKey <plist::String> ("BuiltinJambaseRuleName"),
        plist::MakeKey <plist::Array> ("FileTypes"),
        plist::MakeKey <plist::Array> ("InputFileTypes"),
        plist::MakeKey <plist::Array> ("Outputs"),
        plist::MakeKey <plist::Array> ("Architectures"),
        plist::MakeKey <plist::Dictionary> ("EnvironmentVariables"),
        plist::MakeKey <plist::Object> ("CommandOutputParser"),
        plist::MakeKey <plist::Boolean> ("IsAbstract"),
        plist::MakeKey <plist::Boolean> ("IsArchitectureNeutral"),
        plist::MakeKey <plist::Boolean> ("CaresAboutInclusionDependencies"),
        plist::MakeKey <plist::Boolean> ("SynthesizeBuildRule"),
        plist::MakeKey <plist::Boolean> ("ShouldRerunOnError"),
        plist::MakeKey <plist::Array> ("Options"),
        plist::MakeKey <plist::Array> ("DeletedProperties"),
        // Compiler
        plist::MakeKey <plist::String> ("ExecCPlusPlusLinkerPath"),
        plist::MakeKey <plist::String> ("ExecutionDescription"),
        plist::MakeKey <plist::String> ("SourceFileOption"),
        plist::MakeKey <plist::String> ("OutputDir"),
        plist::MakeKey <plist::String> ("OutputFileExtension"),
        plist::MakeKey <plist::String> ("CommandResultsPostprocessor"),
        plist::MakeKey <plist::String> ("GeneratedInfoPlistContentFilePath"),
        plist::MakeKey <plist::String> ("DependencyInfoFile"),
        plist::MakeKey <plist::Array> ("DependencyInfoArgs"),
        plist::MakeKey <plist::Array> ("Languages"),
        plist::MakeKey <plist::Array> ("OptionConditionFlavors"),
        plist::MakeKey <plist::Array> ("PatternsOfFlagsNotAffectingPrecomps"),
        plist::MakeKey <plist::Array> ("MessageCategoryInfoOptions"),
        plist::MakeKey <plist::Array> ("SynthesizeBuildRuleForBuildPhases"),
        plist::MakeKey <plist::Array> ("InputFileGroupings"),
        plist::MakeKey <plist::Array> ("FallbackTools"),
        plist::MakeKey <plist::Dictionary> ("OverridingProperties"),
        plist::MakeKey <plist::Boolean> ("UseCPlusPlusCompilerDriverWhenBundlizing"),
        plist::MakeKey <plist::Boolean> ("DashIFlagAcceptsHeadermaps"),
        plist::MakeKey <plist::Boolean> ("SupportsHeadermaps"),
        plist::MakeKey <plist::Boolean> ("SupportsIsysroot"),
        plist::MakeKey <plist::Boolean> ("SupportsSeparateUserHeaderPaths"),
        plist::MakeKey <plist::Boolean> ("SupportsGeneratePreprocessedFile"),
        plist::MakeKey <plist::Boolean> ("SupportsGenerateAssemblyFile"),
        plist::MakeKey <plist::Boolean> ("SupportsAnalyzeFile"),
        plist::MakeKey <plist::Boolean> ("SupportsSerializedDiagnostics"),
        plist::MakeKey <plist::Boolean> ("SupportsPredictiveCompilation"),
        plist::MakeKey <plist::Boolean> ("SupportsMacOSXDeploymentTarget"),
        plist::MakeKey <plist::Boolean> ("SupportsMacOSXMinVersionFlag"),
        plist::MakeKey <plist::Boolean> ("PrunePrecompiledHeaderCache"),
        plist::MakeKey <plist::Boolean> ("OutputsAreProducts"),
        plist::MakeKey <plist::Boolean> ("OutputsAreSourceFiles"),
        plist::MakeKey <plist::Boolean> ("SoftError"),
        plist::MakeKey <plist::Boolean> ("DeeplyStatInputDirectories"),
        plist::MakeKey <plist::Boolean> ("DontProcessOutputs"),
        plist::MakeKey <plist::Boolean> ("ShowInCompilerSelectionPopup"),
        plist::MakeKey <plist::Boolean> ("ShowOnlySelfDefinedProperties"));

    if (!Tool::parse(dict, false))
        return false;

    auto ED       = dict->value <plist::String> ("ExecutionDescription");
    auto ECPPLP   = dict->value <plist::String> ("ExecCPlusPlusLinkerPath");
    auto SFO      = dict->value <plist::String> ("SourceFileOption");
    auto OD       = dict->value <plist::String> ("OutputDir");
    auto OFE      = dict->value <plist::String> ("OutputFileExtension");
    auto CRP      = dict->value <plist::String> ("CommandResultsPostprocessor");
    auto GIPCFP   = dict->value <plist::String> ("GeneratedInfoPlistContentFilePath");
    auto DIF      = dict->value <plist::String> ("DependencyInfoFile");
    auto DIAs     = dict->value <plist::Array> ("DependencyInfoArgs");
    auto Ls       = dict->value <plist::Array> ("Languages");
    auto OCFs     = dict->value <plist::Array> ("OptionConditionFlavors");
    auto POFNAPs  = dict->value <plist::Array> ("PatternsOfFlagsNotAffectingPrecomps");
    auto MCIOs    = dict->value <plist::Array> ("MessageCategoryInfoOptions");
    auto SBRFBPs  = dict->value <plist::Array> ("SynthesizeBuildRuleForBuildPhases");
    auto IFGs     = dict->value <plist::Array> ("InputFileGroupings");
    auto FTs      = dict->value <plist::Array> ("FallbackTools");
    auto OP       = dict->value <plist::Dictionary> ("OverridingProperties");
    auto UCPPCDWB = dict->value <plist::Boolean> ("UseCPlusPlusCompilerDriverWhenBundlizing");
    auto DIFAH    = dict->value <plist::Boolean> ("DashIFlagAcceptsHeadermaps");
    auto SH       = dict->value <plist::Boolean> ("SupportsHeadermaps");
    auto SI       = dict->value <plist::Boolean> ("SupportsIsysroot");
    auto SSUHP    = dict->value <plist::Boolean> ("SupportsSeparateUserHeaderPaths");
    auto SGPF     = dict->value <plist::Boolean> ("SupportsGeneratePreprocessedFile");
    auto SGAF     = dict->value <plist::Boolean> ("SupportsGenerateAssemblyFile");
    auto SAF      = dict->value <plist::Boolean> ("SupportsAnalyzeFile");
    auto SSD      = dict->value <plist::Boolean> ("SupportsSerializedDiagnostics");
    auto SPC      = dict->value <plist::Boolean> ("SupportsPredictiveCompilation");
    auto SMOSXDT  = dict->value <plist::Boolean> ("SupportsMacOSXDeploymentTarget");
    auto SMOSXMVF = dict->value <plist::Boolean> ("SupportsMacOSXMinVersionFlag");
    auto PPHC     = dict->value <plist::Boolean> ("PrunePrecompiledHeaderCache");
    auto OAP      = dict->value <plist::Boolean> ("OutputsAreProducts");
    auto OASF     = dict->value <plist::Boolean> ("OutputsAreSourceFiles");
    auto SE       = dict->value <plist::Boolean> ("SoftError");
    auto DSID     = dict->value <plist::Boolean> ("DeeplyStatInputDirectories");
    auto DPO      = dict->value <plist::Boolean> ("DontProcessOutputs");
    auto SICSP    = dict->value <plist::Boolean> ("ShowInCompilerSelectionPopup");
    auto SOSDP    = dict->value <plist::Boolean> ("ShowOnlySelfDefinedProperties");

    if (ECPPLP != nullptr) {
        _execCPlusPlusLinkerPath = ECPPLP->value();
    }

    if (ED != nullptr) {
        _executionDescription = ED->value();
    }

    if (SFO != nullptr) {
        _sourceFileOption = SFO->value();
    }

    if (OD != nullptr) {
        _outputDir = OD->value();
    }

    if (OFE != nullptr) {
        _outputFileExtension = OFE->value();
    }

    if (CRP != nullptr) {
        _commandResultsPostprocessor = CRP->value();
    }

    if (GIPCFP != nullptr) {
        _generatedInfoPlistContentFilePath = GIPCFP->value();
    }

    if (DIF != nullptr) {
        _dependencyInfoFile = DIF->value();
    }

    if (DIAs != nullptr) {
        for (size_t n = 0; n < DIAs->count(); n++) {
            if (auto DIA = DIAs->value <plist::String> (n)) {
                _dependencyInfoArgs.push_back(DIA->value());
            }
        }
    }

    if (Ls != nullptr) {
        for (size_t n = 0; n < Ls->count(); n++) {
            if (auto L = Ls->value <plist::String> (n)) {
                _languages.push_back(L->value());
            }
        }
    }

    if (OCFs != nullptr) {
        for (size_t n = 0; n < OCFs->count(); n++) {
            if (auto OCF = OCFs->value <plist::String> (n)) {
                _optionConditionFlavors.push_back(OCF->value());
            }
        }
    }

    if (POFNAPs != nullptr) {
        for (size_t n = 0; n < POFNAPs->count(); n++) {
            if (auto POFNAP = POFNAPs->value <plist::String> (n)) {
                _patternsOfFlagsNotAffectingPrecomps.push_back(POFNAP->value());
            }
        }
    }

    if (MCIOs != nullptr) {
        for (size_t n = 0; n < MCIOs->count(); n++) {
            if (auto MCIO = MCIOs->value <plist::String> (n)) {
                _messageCategoryInfoOptions.push_back(MCIO->value());
            }
        }
    }

    if (SBRFBPs != nullptr) {
        for (size_t n = 0; n < SBRFBPs->count(); n++) {
            if (auto SBRFBP = SBRFBPs->value <plist::String> (n)) {
                _synthesizeBuildRuleForBuildPhases.push_back(SBRFBP->value());
            }
        }
    }

    if (IFGs != nullptr) {
        for (size_t n = 0; n < IFGs->count(); n++) {
            if (auto IFG = IFGs->value <plist::String> (n)) {
                _inputFileGroupings.push_back(IFG->value());
            }
        }
    }

    if (FTs != nullptr) {
        for (size_t n = 0; n < FTs->count(); n++) {
            if (auto FT = FTs->value <plist::String> (n)) {
                _fallbackTools.push_back(FT->value());
            }
        }
    }

    if (OP != nullptr) {
        if (_overridingProperties != nullptr) {
            _overridingProperties->release();
        }

        _overridingProperties = plist::CastTo <plist::Dictionary> (OP->copy());
    }

    if (UCPPCDWB != nullptr) {
        _useCPlusPlusCompilerDriverWhenBundlizing = UCPPCDWB->value();
    }
    
    if (SH != nullptr) {
        _supportsHeadermaps = SH->value();
    }

    if (SI != nullptr) {
        _supportsIsysroot = SI->value();
    }

    if (SSUHP != nullptr) {
        _supportsSeparateUserHeaderPaths = SSUHP->value();
    }

    if (SGPF != nullptr) {
        _supportsGeneratePreprocessedFile = SGPF->value();
    }

    if (SGAF != nullptr) {
        _supportsGenerateAssemblyFile = SGAF->value();
    }

    if (SAF != nullptr) {
        _supportsAnalyzeFile = SAF->value();
    }

    if (SSD != nullptr) {
        _supportsSerializedDiagnostics = SSD->value();
    }

    if (SPC != nullptr) {
        _supportsPredictiveCompilation = SPC->value();
    }

    if (SMOSXDT != nullptr) {
        _supportsMacOSXDeploymentTarget = SMOSXDT->value();
    }

    if (SMOSXMVF != nullptr) {
        _supportsMacOSXMinVersionFlag = SMOSXMVF->value();
    }

    if (PPHC != nullptr) {
        _prunePrecompiledHeaderCache = PPHC->value();
    }

    if (OAP != nullptr) {
        _outputAreProducts = OAP->value();
    }

    if (OASF != nullptr) {
        _outputAreSourceFiles = OASF->value();
    }

    if (SE != nullptr) {
        _softError = SE->value();
    }

    if (DSID != nullptr) {
        _deeplyStatInputDirectories = DSID->value();
    }

    if (DPO != nullptr) {
        _dontProcessOutputs = DPO->value();
    }

    if (SICSP != nullptr) {
        _showInCompilerSelectionPopup = SICSP->value();
    }

    if (SOSDP != nullptr) {
        _showOnlySelfDefinedProperties = SOSDP->value();
    }

    return true;
}