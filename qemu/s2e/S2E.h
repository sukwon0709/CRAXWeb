/*
 * S2E Selective Symbolic Execution Framework
 *
 * Copyright (c) 2010, Dependable Systems Laboratory, EPFL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Dependable Systems Laboratory, EPFL nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE DEPENDABLE SYSTEMS LABORATORY, EPFL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Currently maintained by:
 *    Volodymyr Kuznetsov <vova.kuznetsov@epfl.ch>
 *    Vitaly Chipounov <vitaly.chipounov@epfl.ch>
 *
 * All contributors are listed in S2E-AUTHORS file.
 *
 */

#ifndef S2E_H
#define S2E_H

#include <fstream>
#include <string>
#include <vector>
//#include <tr1/unordered_map>
#include <map>

#include "Plugin.h"
#include "Synchronization.h"

namespace klee {
    class Interpreter;
    class InterpreterHandler;
}

class TCGLLVMContext;

namespace s2e {

class Plugin;
class CorePlugin;
class ConfigFile;
class PluginsFactory;

class S2EHandler;
class S2EExecutor;
class S2EExecutionState;

class Database;

struct S2EShared {
    unsigned currentProcessCount;
    unsigned lastFileId;
    //We must have unique state ids across all processes
    //otherwise offline tools will be extremely confused when
    //aggregating different execution trace files.
    unsigned lastStateId;
};

class S2E
{
protected:
    S2ESynchronizedObject<S2EShared> m_sync;
    ConfigFile* m_configFile;
    PluginsFactory* m_pluginsFactory;

    CorePlugin* m_corePlugin;
    std::vector<Plugin*> m_activePluginsList;

    typedef std::map<std::string, Plugin*> ActivePluginsMap;
    ActivePluginsMap m_activePluginsMap;

    std::string m_outputDirectory;

    std::ostream*   m_infoFile;

    std::ostream*   m_debugFile;

    std::ostream*   m_messagesFile;
    std::streambuf* m_messagesStreamBuf;

    std::ostream*   m_warningsFile;
    std::streambuf* m_warningsStreamBuf;

    Database *m_database;

    TCGLLVMContext *m_tcgLLVMContext;

    /* How many processes can S2E fork */
    unsigned m_maxProcesses;
    unsigned m_currentProcessIndex;

    std::string m_outputDirectoryBase;

    /* The following members are late-initialized when
    QEMU pc creation is complete */
    S2EHandler* m_s2eHandler;
    S2EExecutor* m_s2eExecutor;

    /* forked indicates whether the current S2E process was forked from a parent S2E process */
    void initOutputDirectory(const std::string& outputDirectory, int verbose, bool forked);

    void initKleeOptions();
    void initExecutor();
    void initPlugins();

    std::ostream& getStream(std::ostream& stream,
                            const S2EExecutionState* state) const;

public:
    /** Construct S2E */
    explicit S2E(int argc, char** argv,
                 TCGLLVMContext* tcgLLVMContext,
                 const std::string& configFileName,
                 const std::string& outputDirectory,
                 int verbose = 0, unsigned s2e_max_processes = 1);
    ~S2E();

    /*****************************/
    /* Configuration and plugins */

    /** Get configuration file */
    ConfigFile* getConfig() const { return m_configFile; }

    /** Get plugin by name of functionName */
    Plugin* getPlugin(const std::string& name) const;

    /** Get Core plugin */
    CorePlugin* getCorePlugin() const { return m_corePlugin; }

    /** Get database */
    Database *getDb() const {
        return m_database;
    }

    /*************************/
    /* Directories and files */

    /** Get output directory name */
    const std::string& getOutputDirectory() const { return m_outputDirectory; }

    /** Get a filename inside an output directory */
    std::string getOutputFilename(const std::string& fileName);

    /** Create output file in an output directory */
    std::ostream* openOutputFile(const std::string &filename);

    /** Get info stream (used only by KLEE internals) */
    std::ostream& getInfoStream(const S2EExecutionState* state = 0) const {
        return getStream(*m_infoFile, state);
    }

    /** Get debug stream (used for non-important debug info) */
    std::ostream& getDebugStream(const S2EExecutionState* state = 0) const {
        return getStream(*m_debugFile, state);
    }

    /** Get messages stream (used for non-critical information) */
    std::ostream& getMessagesStream(const S2EExecutionState* state = 0) const {
        return getStream(*m_messagesFile, state);
    }

    /** Get warnings stream (used for warnings, duplicated on the screen) */
    std::ostream& getWarningsStream(const S2EExecutionState* state = 0) const {
        return getStream(*m_warningsFile, state);
    }

    static void printf(std::ostream &os, const char *fmt, ...);

    /***********************/
    /* Runtime information */
    S2EExecutor* getExecutor() { return m_s2eExecutor; }

    //XXX: A plugin can hold cached state information. When a state is deleted,
    //remove all the cached info from all plugins.
    void refreshPlugins();

    void writeBitCodeToFile();

    int fork();
    unsigned fetchAndIncrementStateId();
};

} // namespace s2e

#endif // S2E_H
