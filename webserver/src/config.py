import dotenv
import os
import urllib.parse

dotenv.load_dotenv()

class Config:
    DEBUG = bool(os.getenv('DEBUG', False))
    DATABASE_USERNAME = os.getenv('DATABASE_USERNAME', "")
    DATABASE_PASSWORD = os.getenv('DATABASE_PASSWORD', "")
    DATABASE_HOSTNAME = os.getenv('DATABASE_HOSTNAME', "localhost")
    DATABASE_PORT = os.getenv('DATABASE_PORT', "5432")
    DATABASE_NAME = os.getenv('DATABASE_NAME', "postgres")
    # Use urllib.parse.quote_plus to escape special characters in the password
    SQLALCHEMY_DATABASE_URI = f"postgresql+psycopg2://{DATABASE_USERNAME}:{urllib.parse.quote_plus(DATABASE_PASSWORD)}@{DATABASE_HOSTNAME}:{DATABASE_PORT}/{DATABASE_NAME}"