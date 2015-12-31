

/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_Context_h
#define __pbxbuild_Phase_Context_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Phase/File.h>
#include <pbxbuild/Tool/Context.h>

namespace pbxbuild {

namespace Tool {
    class ClangResolver;
    class CopyResolver;
    class InfoPlistResolver;
    class MakeDirectoryResolver;
    class ScriptResolver;
    class SymlinkResolver;
    class TouchResolver;
    class ToolResolver;
}

namespace Phase {

class Environment;

class Context {
private:
    Tool::Context                                       _toolContext;

private:
    std::unique_ptr<Tool::ClangResolver>                _clangResolver;
    std::unique_ptr<Tool::CopyResolver>                 _copyResolver;
    std::unique_ptr<Tool::InfoPlistResolver>            _infoPlistResolver;
    std::unique_ptr<Tool::MakeDirectoryResolver>        _makeDirectoryResolver;
    std::unique_ptr<Tool::ScriptResolver>               _scriptResolver;
    std::unique_ptr<Tool::SymlinkResolver>              _symlinkResolver;
    std::unique_ptr<Tool::TouchResolver>                _touchResolver;
    std::unordered_map<std::string, Tool::ToolResolver> _toolResolvers;

public:
    explicit Context(Tool::Context const &toolContext);
    ~Context();

public:
    Tool::Context const &toolContext() const
    { return _toolContext; }

public:
    Tool::Context &toolContext()
    { return _toolContext; }

public:
    Tool::ClangResolver const         *clangResolver(Phase::Environment const &phaseEnvironment);
    Tool::CopyResolver const          *copyResolver(Phase::Environment const &phaseEnvironment);
    Tool::InfoPlistResolver const     *infoPlistResolver(Phase::Environment const &phaseEnvironment);
    Tool::MakeDirectoryResolver const *makeDirectoryResolver(Phase::Environment const &phaseEnvironment);
    Tool::ScriptResolver const        *scriptResolver(Phase::Environment const &phaseEnvironment);
    Tool::SymlinkResolver const       *symlinkResolver(Phase::Environment const &phaseEnvironment);
    Tool::TouchResolver const         *touchResolver(Phase::Environment const &phaseEnvironment);
    Tool::ToolResolver const          *toolResolver(Phase::Environment const &phaseEnvironment, std::string const &identifier);

public:
    bool resolveBuildFiles(
        Phase::Environment const &phaseEnvironment,
        pbxsetting::Environment const &environment,
        pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase,
        std::string const &outputDirectory,
        std::vector<Phase::File> const &files,
        std::string const &fallbackToolIdentifier = std::string());

private:
    bool resolveBuildFile(
        Phase::Environment const &phaseEnvironment,
        pbxsetting::Environment const &environment,
        pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase,
        std::string const &outputDirectory,
        Phase::File const &file,
        std::string const &fallbackToolIdentifier);
};

}
}

#endif // !__pbxbuild_Phase_Context_h