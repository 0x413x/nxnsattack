#include "postsignedzone.hpp"
#include "postsignedzoneimp.hpp"

namespace dns
{
    PostSignedZone::PostSignedZone( const Domainname &zone_name, const std::string &ksk_config, const std::string &zsk_config )
	: mImp( new PostSignedZoneImp( zone_name, ksk_config, zsk_config ) )
    {}

    void PostSignedZone::add( std::shared_ptr<RRSet> rrset )
    {
	mImp->add( rrset );
    }

    PacketInfo PostSignedZone::getAnswer( const PacketInfo &query ) const
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

    PostSignedZone::RRSetPtr PostSignedZone::getSOA() const
    {
	return mImp->getSOA();
    }

    PostSignedZone::RRSetPtr PostSignedZone::getNameServer() const
    {
	return mImp->getNameServer();
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