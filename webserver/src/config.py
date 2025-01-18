import dotenv
import os

dotenv.load_dotenv()

class Config:
    DEBUG = bool(os.getenv('DEBUG', False))
    SQLALCHEMY_DATABASE_URI = os.getenv('SQLALCHEMY_DATABASE_URI')