#include "Federate.h"
#include <iostream>

Federate::Federate()
{
	rtiAmbassadorFactory = NULL;
	isConnected = false;
}

Federate::~Federate(void)
{
}

bool Federate::createAndJoinFederation(std::wstring& federationName, std::wstring& federateName, std::wstring& fomFile)
{
	rtiAmbassadorFactory = new rti1516e::RTIambassadorFactory();
	//create ambassador
	try
	{
		//std::vector< std::wstring > args;
		rtiAmbassador = rtiAmbassadorFactory->createRTIambassador();
		rtiAmbassador->connect(*this, rti1516e::HLA_EVOKED);
	}
	catch (rti1516e::Exception& e)
	{
		rtiAmbassador.reset();
		std::wcout << L"Error: " << e.what() << std::endl;
		return false;
	}

	// Create federation
	try
	{
		rtiAmbassador->createFederationExecution(federationName, fomFile);
	}
	catch (rti1516e::FederationExecutionAlreadyExists&)
	{
		// No problem if it already exists
	}
	catch (rti1516e::Exception& e)
	{
		std::wcout << L"Error: " << e.what() << std::endl;
		return false;
	}

	// Join federation
	try
	{
		rtiAmbassador->evokeMultipleCallbacks(0.1, 0.2);
		federateHandle = rtiAmbassador->joinFederationExecution(federateName, federationName/*, *this*/);
		rtiAmbassador->evokeMultipleCallbacks(0.1, 0.2);
	}
	catch (rti1516e::Exception& e)
	{
		std::wcout << L"Error: " << e.what() << std::endl;
		return false;
	}

	this->federateName = federateName;
	this->federationName = federationName;

	isConnected = true;
	return true;
}

bool Federate::resignAndDestroyFederation()
{
	try
	{
		rtiAmbassador->resignFederationExecution(rti1516e::DELETE_OBJECTS);
	}
	catch (rti1516e::Exception& e)
	{
		std::wcout << "Resign Error: " << e.what() << std::endl;
		return false;
	}

	try
	{
		rtiAmbassador->destroyFederationExecution(federationName);
	}
	catch (rti1516e::FederatesCurrentlyJoined&)
	{
		// No problem if other federates still exist. Let them deal with it.
	}
	catch (rti1516e::Exception& e)
	{
		std::wcout << "Destroy Error: " << e.what() << std::endl;
		return false;
	}

	isConnected = false;
	return true;
}

bool Federate::publishAndSubscribeAttributes()
{
	try
	{
		rti1516e::ObjectClassHandle classhandle = rtiAmbassador->getObjectClassHandle(L"BaseEntity.PhysicalEntity.Platform.Aircraft");

		rti1516e::AttributeHandleSet handleSet;
		rti1516e::AttributeHandle attributeHandle1 = rtiAmbassador->getAttributeHandle(classhandle, L"ForceIdentifier");
		rti1516e::AttributeHandle attributeHandle2 = rtiAmbassador->getAttributeHandle(classhandle, L"WorldLocation");
		rti1516e::AttributeHandle attributeHandle3 = rtiAmbassador->getAttributeHandle(classhandle, L"Orientation");
		rti1516e::AttributeHandle attributeHandle4 = rtiAmbassador->getAttributeHandle(classhandle, L"HatchState");
		rti1516e::AttributeHandle attributeHandle5 = rtiAmbassador->getAttributeHandle(classhandle, L"EngineSmokeOn");
		rti1516e::AttributeHandle attributeHandle6 = rtiAmbassador->getAttributeHandle(classhandle, L"TailLightsOn");

		handleSet.insert(attributeHandle1);
		handleSet.insert(attributeHandle2);
		handleSet.insert(attributeHandle3);
		handleSet.insert(attributeHandle4);
		handleSet.insert(attributeHandle5);
		handleSet.insert(attributeHandle6);

		rtiAmbassador->subscribeObjectClassAttributes(classhandle, handleSet);


	}
	catch (rti1516e::Exception& e)
	{
		std::wcout << "Subscription Error: " << e.what() << std::endl;
		return false;
	}

	return true;
}

void Federate::swapData(unsigned char* data, int size)
{
	if (size <= 1)
		return;

	register int i = 0;
	register int j = size - 1;
	while (i<j)
	{
		std::swap(data[i], data[j]);
		i++;
		j--;
	}
}
void Federate::evoke()
{
	rtiAmbassador->evokeMultipleCallbacks(0.1, 0.2);
}

// NullFederateAmbassador
void Federate::discoverObjectInstance(rti1516e::ObjectInstanceHandle objectInstanceHandle, rti1516e::ObjectClassHandle objectClassHandle, std::wstring const& objectInstanceName) throw(rti1516e::FederateInternalError)
{
	auto className = rtiAmbassador->getObjectClassName(objectClassHandle);
	std::wcout << L"Object Created (Class:" << className << L") (InstanceName:" << objectInstanceName << ")" << std::endl;
}

void Federate::discoverObjectInstance(rti1516e::ObjectInstanceHandle objectInstanceHandle, rti1516e::ObjectClassHandle objectClassHandle, std::wstring const& objectInstanceName, rti1516e::FederateHandle producingFederate) throw(rti1516e::FederateInternalError)
{
	auto className = rtiAmbassador->getObjectClassName(objectClassHandle);
	std::wcout << L"Object Created (Class:" << className << L") (InstanceName:" << objectInstanceName << ")" << std::endl;
}

void Federate::removeObjectInstance(rti1516e::ObjectInstanceHandle object, rti1516e::VariableLengthData const& userSuppliedTag, rti1516e::OrderType sentOrder, rti1516e::SupplementalRemoveInfo theRemoveInfo) throw(rti1516e::FederateInternalError)
{
	auto objectInstanceName = rtiAmbassador->getObjectInstanceName(object);
	auto objectClassHandle = rtiAmbassador->getKnownObjectClassHandle(object);
	auto className = rtiAmbassador->getObjectClassName(objectClassHandle);
	std::wcout << L"Object Removed (Class:" << className << L") (InstanceName:" << objectInstanceName << ")" << std::endl;
}
void Federate::removeObjectInstance(rti1516e::ObjectInstanceHandle object, rti1516e::VariableLengthData const& userSuppliedTag, rti1516e::OrderType sentOrder, rti1516e::LogicalTime const& time, rti1516e::OrderType receivedOrder, rti1516e::SupplementalRemoveInfo theRemoveInfo) throw(rti1516e::FederateInternalError)
{
	auto objectInstanceName = rtiAmbassador->getObjectInstanceName(object);
	auto objectClassHandle = rtiAmbassador->getKnownObjectClassHandle(object);
	auto className = rtiAmbassador->getObjectClassName(objectClassHandle);
	std::wcout << L"Object Removed (Class:" << className << L") (InstanceName:" << objectInstanceName << ")" << std::endl;
}
void Federate::removeObjectInstance(rti1516e::ObjectInstanceHandle object, rti1516e::VariableLengthData const& userSuppliedTag, rti1516e::OrderType sentOrder, rti1516e::LogicalTime const& time, rti1516e::OrderType receivedOrder, rti1516e::MessageRetractionHandle handle, rti1516e::SupplementalRemoveInfo theRemoveInfo) throw(rti1516e::FederateInternalError)
{
	auto objectInstanceName = rtiAmbassador->getObjectInstanceName(object);
	auto objectClassHandle = rtiAmbassador->getKnownObjectClassHandle(object);
	auto className = rtiAmbassador->getObjectClassName(objectClassHandle);
	std::wcout << L"Object Removed (Class:" << className << L") (InstanceName:" << objectInstanceName << ")" << std::endl;
}

void Federate::reflectAttributeValues(rti1516e::ObjectInstanceHandle object, rti1516e::AttributeHandleValueMap const& attributeValues, rti1516e::VariableLengthData const& userSuppliedTag, rti1516e::OrderType sentOrder, rti1516e::TransportationType type, rti1516e::SupplementalReflectInfo reflectInfo) throw (rti1516e::FederateInternalError)
{
	handleAttributeReflection(object, attributeValues);
}
void Federate::reflectAttributeValues(rti1516e::ObjectInstanceHandle object, rti1516e::AttributeHandleValueMap const& attributeValues, rti1516e::VariableLengthData const& userSuppliedTag, rti1516e::OrderType sentOrder, rti1516e::TransportationType type, rti1516e::LogicalTime const & time, rti1516e::OrderType receivedOrder, rti1516e::SupplementalReflectInfo reflectInfo) throw (rti1516e::FederateInternalError)
{
	handleAttributeReflection(object, attributeValues);
}
void Federate::reflectAttributeValues(rti1516e::ObjectInstanceHandle object, rti1516e::AttributeHandleValueMap const& attributeValues, rti1516e::VariableLengthData const& userSuppliedTag, rti1516e::OrderType sentOrder, rti1516e::TransportationType type, rti1516e::LogicalTime const & time, rti1516e::OrderType receivedOrder, rti1516e::MessageRetractionHandle theHandle, rti1516e::SupplementalReflectInfo reflectInfo) throw (rti1516e::FederateInternalError)
{
	handleAttributeReflection(object, attributeValues);
}

void Federate::handleAttributeReflection(rti1516e::ObjectInstanceHandle object, rti1516e::AttributeHandleValueMap const& attributes)
{
	for (auto it = attributes.begin(); it != attributes.end(); ++it)
	{
		auto attributeHandle = it->first;
		auto objectInstanceName = rtiAmbassador->getObjectInstanceName(object);
		auto objectClassHandle = rtiAmbassador->getKnownObjectClassHandle(object);
		auto className = rtiAmbassador->getObjectClassName(objectClassHandle);
		auto attributeName = rtiAmbassador->getAttributeName(objectClassHandle, attributeHandle);
		if (attributeName == L"ForceIdentifier")
		{
			char forceIdentifier = *(static_cast<const char*>(it->second.data()));
			std::wstring value;
			
			switch (forceIdentifier) {
			case 0: value = L"Other"; break;
			case 1: value = L"Friendly"; break;
			case 2: value = L"Opposing"; break;
			case 3: value = L"Neutral"; break;
			default: value = L"UNKNOWN"; break;
			}
			std::wcout << L"Attribute Reflected (Class:" << className << L") (InstanceName:" << objectInstanceName
				<< L") (ForceIdentifier:" << value << ")" << std::endl;

		}
		else if (attributeName == L"WorldLocation")
		{
			double x, y, z;
			char* data = (char*)(it->second.data());
			memcpy(&x, data, sizeof(double));
			data += sizeof(double);
			memcpy(&y, data, sizeof(double));
			data += sizeof(double);
			memcpy(&z, data, sizeof(double));
			data += sizeof(double);
			SwapLittleEndian(x);
			SwapLittleEndian(y);
			SwapLittleEndian(z);
			std::wcout << L"Attribute Reflected (Class:" << className << L") (InstanceName:" << objectInstanceName
				<< L") (WorldLocation: (x:" << x << L") (y:" << y << L") (z:" << z << "))" << std::endl;

		}

		else if (attributeName == L"Orientation")
		{
			
			// X means Psi, Y means Theta, Z means Phi

			float x, y, z;
			char* data = (char*)(it->second.data());
			memcpy(&x, data, sizeof(float));
			data += sizeof(float);
			memcpy(&y, data, sizeof(float));
			data += sizeof(float);
			memcpy(&z, data, sizeof(float));
			data += sizeof(float);
			SwapLittleEndian(x);
			SwapLittleEndian(y);
			SwapLittleEndian(z);
			std::wcout << L"Attribute Reflected (Class:" << className << L") (InstanceName:" << objectInstanceName
				<< L") (Orientation: (psi:" << x << L") (theta:" << y << L") (phi:" << z << "))" << std::endl;
		}

		else if (attributeName == L"HatchState")
		{ // 
			int HatchState = *(static_cast<const int*>(it->second.data()));
			SwapLittleEndian(HatchState);
			std::wstring value;
			switch (HatchState) {
			case 0: value = L"Not Applicable"; break;
			case 1: value = L"Primary Hatch is Closed"; break;
			case 2: value = L"Primary Hatch is Popped"; break;
			case 3: value = L"Primary hatch is popped and a Person is Visible Under Hatch"; break;
			case 4: value = L"Primary hatch is Opened"; break;
			case 5: value = L"Primary hatch is Open and Person is Visible"; break;
			default: value = L"UNKNOWN"; break;
			}

			std::wcout << L"Attribute Reflected (Class:" << className << L") (InstanceName:" << objectInstanceName<< ") (HatchState:" << value << L")" << std::endl;
		}

		else if (attributeName == L"EngineSmokeOn")
		{  
			bool value = *(static_cast<const bool*>(it->second.data()));
			std::wcout << L"Attribute Reflected (Class:" << className << L") (InstanceName:" << objectInstanceName
				<< L") (EngineSmokeOn: (x:" << value << L"))" << std::endl;
		}

		else if (attributeName == L"TailLightsOn")
		{
			bool value = *(static_cast<const bool*>(it->second.data()));
			std::wcout << L"Attribute Reflected (Class:" << className << L") (InstanceName:" << objectInstanceName
				<< ") (TailLightsOn: (x:" << value << L"))" << std::endl;
		}

	}
}