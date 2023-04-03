# AI Colosseum Backend App

### 1. Install dependencies:

* PostgreSQL
* Redis-server

### 2. Create and configure the database and user:

Create a new user for our database using psql. For the password choose 'pwd':

```bash
createuser -P colosseumdb_admin
```

Create the database for our user:

```bash
createdb -O colosseumdb_admin colosseumdb
```

Connect to our database as a default postgres user:

```
psql -d colosseumdb
```

Create the schema using the 'make_schema.sql' file (it may be necessary to use the absolute path of the make_schema.sql file)

```bash
\include database/make_schema.sql
```

You can also create some examples using the 'make_example_data.sql' file (it may be necessary to use the absolute path of the file)

```bash
\include database/make_example_data.sql
```

Grant privileges to our colosseumdb admin user:

```mysql
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO colosseumdb_admin; 
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO colosseumdb_admin;
```

**You can use different names for the database and user with a different password, but then you need to change the config/dbconfig.toml file**

### 3. Prepare the environment

**Method 1 (recommended)**

Install anaconda or miniconda and execute the following commands:

```
conda env create -f environment.yml
conda activate backend
```

**Method 2**

Manually install Python 3.8 and Pip

Install the required Python packages from requirements:

```
pip install -r requirements.txt
```


### 4. Start the API:

Create redis instances:

```
make redis
```

Run the api with uvicorn:

```
make serve
```

Test the API using the generated swagger docs: http://127.0.0.1:8000/docs# 
