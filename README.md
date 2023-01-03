<img align="right" src="https://cdn-icons-png.flaticon.com/512/6124/6124995.png" width="200">

# SoBay
Auction System for Linux, with a server/client architecture.
<br><br>
## Features 📑
* Backend: The application responsible for responding to all the requests from the clients
* Frontend: The application where all clients login to participate in the action
> No Frontend can be initialized without the Backend being ON

<br>

## How to Use 🛠
In the terminal use the following command: 
<br>
`make rr` - This will initialize/compile all the programms needed
<br>
`make clear` - This will clear all the temporary files and the `.exe` files
<br><br>
`./backend` - to initialize the backend application
<br>
`./frontend <username> <password>` - to initialize the frontend application
<br>
###### It's required 3 ambient variables: `FUSERS`, `FITENS` and `FPROMOTORES`. They're gonna store the path for each file.
<br>

## Commands 💻 (Backend)
* `users` : prints all the users currently online
* `kick <username>` : kicks the user
* `list` : lists all the items for sale
* `prom` : list all the promotions active
* `reprom` : reload the promotions programs based on the txt file
* `cancel` : close the exe of one of the promotors program
* `time` : prints the system time in seconds
* `help` : prints a command list
* `close` :  closes the backend and all the frontends
<br><br>
## Commands 👥 (Frontend)
* `list` : lists all the items for sale
* `lisel <name>` : lists all the items from a seller
* `lival <value>` : lists all the items with a price <= `value`
* `litime <time>` : lists all the items with time <= `time`
* `buy <id> <amount>` : tries to buy and item by its id
* `sell <name> <categorie> <price> <price-buy-now> <time>` : puts a product up for sale
* `cash` : prints the amount of money the user has
* `add <amount>` : adds an amount to the users money
* `time` : prints the system time in seconds
* `help` : prints a command list
* `exit` :  closes the frontend and notifies the backend

##
