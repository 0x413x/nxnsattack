#ifndef UNSIGNED_ZONE_HPP
#define UNSIGNED_ZONE_HPP

#include "zone.hpp"

namespace dns
{
    class UnsignedZoneImp;
    
    class UnsignedZone : public AbstractZone
    {
    public:
        UnsignedZone( const Domainname &zone_name, const std::string &ksk_config = "", const std::string &zsk_config = "" );

        void add( std::shared_ptr<RRSet> rrset );
        PacketInfo getAnswer( const PacketInfo &query ) const;
        NodePtr  findNode( const Domainname &domainname ) const;
        RRSetPtr findRRSet( const Domainname &domainname, Type type ) const;
	std::vector<std::shared_ptr<RecordDS>> getDSRecords() const; 
        void verify() const;
        std::shared_ptr<RRSet> signRRSet( const RRSet &rrset );

	RRSetPtr getSOA() const;
	RRSetPtr getNameServer() const;

        static void initialize();
	
    private:
	std::shared_ptr<UnsignedZoneImp> mImp;
    };

}

#endif

