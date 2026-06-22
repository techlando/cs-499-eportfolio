from pymongo import MongoClient
from bson.objectid import ObjectId

class AnimalShelter(object):
        """ CRUD operations for Animal collection in MongoDB """

        def __init__(self, username, password):
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
            HOST = 'nv-desktop-services.apporto.com'
            PORT = 32138
            DB = 'AAC'
            COL = 'animals'
            #
            # Initialize Connection
            #
            self.client = MongoClient('mongodb://%s:%s@%s:%d' % (username,password,HOST,PORT))
            self.database = self.client['%s' % (DB)]
            self.collection = self.database['%s' % (COL)]

    # Complete this create method to implement the C in CRUD.
        def create(self, data):
            if data is not None:
                result = self.database.animals.insert_one(data)  # data should be dictionary  
                return True if result.acknowledged else False   #Return true if method worked else return false       
            else:
                raise Exception("Nothing to save, because data parameter is empty")

    # Create method to implement the R in CRUD.

        def read(self, query):
            results = self.collection.find(query)
            return [doc for doc in results] 
            
    # Method for the U in CRUD (Update)
	
        def update(self, query, new_data):
            if query is not None and new_data is not None:
                try:
                    result = self.collection.update_many(query, {"$set": new_data})
                    return result.modified_count
                except Exception as e:
                    print(f"Update error: {e}")
                    return 0
            else:
                raise Exception("Query and/or new data is empty")
        
    # Method for the D in CRUD (Delete)
    
        def delete(self, query):
            if query is not None:
                try:
                    result = self.collection.delete_many(query)
                    return result.deleted_count
                except Exception as e:
                    print(f"Delete Errors: {e}")
                    return 0
            else:
                raise Exception("Query parameters are empty")
	
	
	
	
	
	
	
	
	
	
	            
