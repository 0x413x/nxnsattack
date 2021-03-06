#include "postsignedzone.hpp"
#include "postsignedzoneimp.hpp"

namespace dns
{
    PostSignedZone::PostSignedZone( const Domainname &zone_name, const std::string &ksk_config, const std::string &zsk_config,
                                    const std::vector<uint8_t> &salt, uint16_t iterate, HashAlgorithm algo,
                                    bool enable_nsec, bool enable_nsec3 )
	: mImp( new PostSignedZoneImp( zone_name, ksk_config, zsk_config, salt, iterate, algo, enable_nsec, enable_nsec3 ) )
    {}

    void PostSignedZone::add( std::shared_ptr<RRSet> rrset )
    {
	mImp->add( rrset );
    }

    MessageInfo PostSignedZone::getAnswer( const MessageInfo &query ) const
    {
	return mImp->getAnswer( query );
    }

    PostSignedZone::NodePtr PostSignedZone::findNode( const Domainname &domainname ) const
    {
        return mImp->findNode( domainname );
    }

    PostSignedZone::RRSetPtr PostSignedZone::findRRSet( const Domainname &domainname, Type type ) const
    {
        return mImp->findRRSet( domainname, type );
    }

    std::vector<std::shared_ptr<RecordDS>> PostSignedZone::getDSRecords() const
    {
	return mImp->getDSRecords();
    }

    void PostSignedZone::verify() const
    {
	mImp->verify();
    }

    void PostSignedZone::setup()
    {
	mImp->setup();
    }

    std::shared_ptr<RRSet> PostSignedZone::signRRSet( const RRSet &rrset )
    {
        return mImp->signRRSet( rrset );
    }

    void PostSignedZone::initialize()
    {
        PostSignedZoneImp::initialize();
    }
}
