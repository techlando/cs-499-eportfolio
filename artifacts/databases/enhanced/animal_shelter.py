from pymongo import MongoClient
from pymongo.errors import PyMongoError
from bson.objectid import ObjectId
import os

class AnimalShelter(object):
        """ CRUD operations for Animal collection in MongoDB """

        def __init__(self, username=None, password=None):
            # Initializing the MongoClient. This helps to 
            # access the MongoDB databases and collections.
            # This is hard-wired to use the aac database, the 
            # animals collection, and the aac user.
            # Definitions of the connection string variables are
            # unique to the individual Apporto environment.
            #
            # You must edit the connection variables below to reflect
            # your own instance of MongoDB!
            #
            # Connection Variables
            #
            # USER = 'aacuser'
            # PASS = '123456'
            HOST = os.getenv('MONGO_HOST', 'nv-desktop-services.apporto.com')
            PORT = int(os.getenv('MONGO_PORT', 32138))
            DB = os.getenv('MONGO_DB', 'AAC')
            COL = os.getenv('MONGO_COLLECTION', 'animals')
            #
            # Initialize Connection
            #

            # Username and password can still be passed from the dashboard,
            # but can also come from environment variables for better security.
            username = username or os.getenv('MONGO_USER')
            password = password or os.getenv('MONGO_PASS')

            if not username or not password:
                raise ValueError("MongoDB username and password are required")

            try:
                self.client = MongoClient(
                    'mongodb://%s:%s@%s:%d' % (username, password, HOST, PORT),
                    serverSelectionTimeoutMS=5000
                )
                self.database = self.client['%s' % (DB)]
                self.collection = self.database['%s' % (COL)]
            except PyMongoError as e:
                print(f"Database connection error: {e}")
                raise

    # Complete this create method to implement the C in CRUD.
        def create(self, data):
            if data is not None and isinstance(data, dict) and len(data) > 0:
                try:
                    result = self.database.animals.insert_one(data)  # data should be dictionary  
                    return True if result.acknowledged else False   #Return true if method worked else return false
                except PyMongoError as e:
                    print(f"Create error: {e}")
                    return False
            else:
                raise ValueError("Nothing to save, because data parameter is empty or invalid")

    # Create method to implement the R in CRUD.

        def read(self, query=None, projection=None):
            if query is None:
                query = {}

            if not isinstance(query, dict):
                raise ValueError("Read query must be a dictionary")

            if projection is not None and not isinstance(projection, dict):
                raise ValueError("Projection must be a dictionary")

            try:
                results = self.collection.find(query, projection)
                return [doc for doc in results]
            except PyMongoError as e:
                print(f"Read error: {e}")
                return []
            
    # Method for the U in CRUD (Update)
	
        def update(self, query, new_data):
            if query is not None and new_data is not None and isinstance(query, dict) and isinstance(new_data, dict) and len(query) > 0 and len(new_data) > 0:
                try:
                    result = self.collection.update_many(query, {"$set": new_data})
                    return result.modified_count
                except PyMongoError as e:
                    print(f"Update error: {e}")
                    return 0
            else:
                raise ValueError("Query and/or new data is empty or invalid")
        
    # Method for the D in CRUD (Delete)
    
        def delete(self, query):
            if query is not None and isinstance(query, dict) and len(query) > 0:
                try:
                    result = self.collection.delete_many(query)
                    return result.deleted_count
                except PyMongoError as e:
                    print(f"Delete Errors: {e}")
                    return 0
            else:
                raise ValueError("Query parameters are empty or invalid")