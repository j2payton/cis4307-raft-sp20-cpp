#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "MetadataStore.hpp"

// A simple ping. Always returns True
class PingMethod : public xmlrpc_c::method {
public:
    PingMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "b:";
        this->_help = "A simple ping method";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {
        
        paramList.verifyEnd(0);

		spdlog::get("stderr")->info("Ping()");

        *retvalP = xmlrpc_c::value_boolean(true);
    }

protected:
	MetadataStore &m;
};

// Gets a block, given a specific hash value
class GetBlockMethod : public xmlrpc_c::method {
public:
    GetBlockMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "6:s";
        this->_help = "Given a hash value, return the associated block";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		// Read in the hash value
		string const h(paramList.getString(0));
        paramList.verifyEnd(1);

		spdlog::get("stderr")->info("GetBlock({})", h);

		unsigned char blockData[] = {0x10, 0x11, 0x12, 0x13, 0x14};
		vector<unsigned char> const block(&blockData[0], &blockData[4]);

        *retvalP = xmlrpc_c::value_bytestring(block);
    }

protected:
	MetadataStore &m;
};

// Puts a block
class PutBlockMethod : public xmlrpc_c::method {
public:
    PutBlockMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "b:6";
        this->_help = "Puts a block in the store";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		// Read in the block
		vector<unsigned char> const b(paramList.getBytestring(0));
        paramList.verifyEnd(1);

		spdlog::get("stderr")->info("PutBlock()");

        *retvalP = xmlrpc_c::value_boolean(true);
    }

protected:
	MetadataStore &m;
};

// Determines whether the server has any of the blocks provided
// Input: a list of blocks
// Output: a subset of the input list where the server has that block
class HasBlocksMethod : public xmlrpc_c::method {
public:
    HasBlocksMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "A:A";
        this->_help = "Returns the subset of hash values stored in this server";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		// Read in the block
		vector<xmlrpc_c::value> const hashlist(paramList.getArray(0));
        paramList.verifyEnd(1);

		spdlog::get("stderr")->info("HasBlocks()");

		vector<xmlrpc_c::value> blocks;
		blocks.push_back(xmlrpc_c::value_string("h0"));
		blocks.push_back(xmlrpc_c::value_string("h1"));
		blocks.push_back(xmlrpc_c::value_string("h2"));

        *retvalP = xmlrpc_c::value_array(blocks);
    }

protected:
	MetadataStore &m;
};

// Retrieves the server's FileInfoMap
// Input: N/A
// Output: A map of filename to (version, hashlist) tuples
class GetFileInfoMapMethod : public xmlrpc_c::method {
public:
    GetFileInfoMapMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "S:";
        this->_help = "Retrieves the server's FileInfoMap";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

        paramList.verifyEnd(0);

		spdlog::get("stderr")->info("GetFileInfomap()");

		map<string, xmlrpc_c::value> fileInfoMap;

		vector<xmlrpc_c::value> f1info;
		f1info.push_back(xmlrpc_c::value_int(3)); // version

		vector<xmlrpc_c::value> f1blocks;
		f1blocks.push_back(xmlrpc_c::value_string("h0"));
		f1blocks.push_back(xmlrpc_c::value_string("h1"));
		f1blocks.push_back(xmlrpc_c::value_string("h2"));

		f1info.push_back(xmlrpc_c::value_array(f1blocks));

		pair<string, xmlrpc_c::value> f1entry("file1.txt", xmlrpc_c::value_array(f1info));
		fileInfoMap.insert(f1entry);

        *retvalP = xmlrpc_c::value_struct(fileInfoMap);
    }

protected:
	MetadataStore &m;
};

// Updates the entry for a given file
// Input: filename, version, and hashlist
// Output: A boolean indicating success (potentially an exception if not)
class UpdateFileMethod : public xmlrpc_c::method {
public:
    UpdateFileMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "b:siA";
        this->_help = "Updates the FileInfo entry of the given file";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		// Filename
		string const filename(paramList.getString(0));

		// Version
		int const version(paramList.getInt(1));

		// Updated hashlist
		vector<xmlrpc_c::value> const hashlist(paramList.getArray(2));

        paramList.verifyEnd(3);

		spdlog::get("stderr")->info("UpdateFile({})", filename);

		(void) version;	// to suppress unused variable warning
		(void) hashlist;// to suppress unused variable warning

        *retvalP = xmlrpc_c::value_boolean(true);
    }

protected:
	MetadataStore &m;
};

////////////////////////////// Project 3 ///////////////////////////////

// Queries whether this metadata store is a leader
// Note that this call should work even when the server is "crashed"
class IsLeaderMethod : public xmlrpc_c::method {
public:
    IsLeaderMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "b:";
        this->_help = "Is this metadata store a leader?";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

        paramList.verifyEnd(0);

		spdlog::get("stderr")->info("IsLeader()");

        *retvalP = xmlrpc_c::value_boolean(false);
    }

protected:
	MetadataStore &m;
};

// "Crashes" this metadata store
// Until Restore() is called, the server should reply to all RPCs
// with an error (unless indicated otherwise), and shouldn't send
// RPCs to other servers
class CrashMethod : public xmlrpc_c::method {
public:
    CrashMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "b:";
        this->_help = "Crashes this metadata store";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

        paramList.verifyEnd(0);

		spdlog::get("stderr")->info("Crash()");

        *retvalP = xmlrpc_c::value_boolean(true);
    }

protected:
	MetadataStore &m;
};

// "Restores" this metadata store, allowing it to start responding
// to and sending RPCs to other nodes
class RestoreMethod : public xmlrpc_c::method {
public:
    RestoreMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "b:";
        this->_help = "Restores this metadata store";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

        paramList.verifyEnd(0);

		spdlog::get("stderr")->info("Restore()");

        *retvalP = xmlrpc_c::value_boolean(true);
    }

protected:
	MetadataStore &m;
};

// "IsCrashed" returns the status of this metadata node (crashed or not)
// This method should always work, even when the node is crashed
class IsCrashedMethod : public xmlrpc_c::method {
public:
    IsCrashedMethod(MetadataStore & mref) : m(mref) {
        this->_signature = "b:";
        this->_help = "Returns whether this node is crashed or not";
    }

    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

        paramList.verifyEnd(0);

		spdlog::get("stderr")->info("IsCrashed()");

        *retvalP = xmlrpc_c::value_boolean(false);
    }

protected:
	MetadataStore &m;
};

int 
main(int argc, char *argv[])
{
	MetadataStore m;
	auto err_logger = spdlog::stderr_color_mt("stderr");
	spdlog::set_level(spdlog::level::debug);
	spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] [thread %t] %v");

	spdlog::get("stderr")->info("Starting RPC server");

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " configfile servernum" << endl;
        exit(1);
    }

    

    try {
        xmlrpc_c::registry myRegistry;

        xmlrpc_c::methodPtr const PingMethodP(new PingMethod(m));
        myRegistry.addMethod("surfstore.ping", PingMethodP);

        xmlrpc_c::methodPtr const GetBlockMethodP(new GetBlockMethod(m));
        myRegistry.addMethod("surfstore.getblock", GetBlockMethodP);

        xmlrpc_c::methodPtr const PutBlockMethodP(new PutBlockMethod(m));
        myRegistry.addMethod("surfstore.putblock", PutBlockMethodP);

        xmlrpc_c::methodPtr const HasBlocksMethodP(new HasBlocksMethod(m));
        myRegistry.addMethod("surfstore.hasblocks", HasBlocksMethodP);

        xmlrpc_c::methodPtr const GetFileInfoMapMethodP(new GetFileInfoMapMethod(m));
        myRegistry.addMethod("surfstore.getfileinfomap", GetFileInfoMapMethodP);

        xmlrpc_c::methodPtr const UpdateFileMethodP(new UpdateFileMethod(m));
        myRegistry.addMethod("surfstore.updatefile", UpdateFileMethodP);

        xmlrpc_c::methodPtr const IsLeaderMethodP(new IsLeaderMethod(m));
        myRegistry.addMethod("surfstore.isLeader", IsLeaderMethodP);

        xmlrpc_c::methodPtr const CrashMethodP(new CrashMethod(m));
        myRegistry.addMethod("surfstore.crash", CrashMethodP);

        xmlrpc_c::methodPtr const RestoreMethodP(new RestoreMethod(m));
        myRegistry.addMethod("surfstore.restore", RestoreMethodP);

        xmlrpc_c::methodPtr const IsCrashedMethodP(new IsCrashedMethod(m));
        myRegistry.addMethod("surfstore.isCrashed", IsCrashedMethodP);
        
        xmlrpc_c::serverAbyss myAbyssServer(
            xmlrpc_c::serverAbyss::constrOpt()
            .registryP(&myRegistry)
            .portNumber(8080));
        
        myAbyssServer.run();
        // xmlrpc_c::serverAbyss.run() never returns
        assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }

    return 0;
}

