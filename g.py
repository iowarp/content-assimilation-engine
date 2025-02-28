import globus_sdk

# Set this.
CLIENT_ID = "01234567-0123-0123-0123-0123456789ab"

client = globus_sdk.NativeAppAuthClient(CLIENT_ID)
client.oauth2_start_flow()
authorize_url = client.oauth2_get_authorize_url()
print(f"Please go to this URL and login:\n\n{authorize_url}\n")

auth_code = input("Please enter the code you get after login here: ").strip()
token_response = client.oauth2_exchange_code_for_tokens(auth_code)

transfer = token_response.by_resource_server["transfer.api.globus.org"]
transfer_token = transfer["access_token"]
authorizer = globus_sdk.AccessTokenAuthorizer(transfer_token)
tc = globus_sdk.TransferClient(authorizer=authorizer)

# Set these.
SOURCE_COLLECTION_ID = "01234567-0123-0123-0123-0123456789ab"
DEST_COLLECTION_ID = "01234567-0123-0123-0123-0123456789ab"

task_data = globus_sdk.TransferData(
    source_endpoint=SOURCE_COLLECTION_ID,
    destination_endpoint=DEST_COLLECTION_ID,
)

src = "/afrl-challenge-data/musinski_afrl_am_package_v2.1/Input Data/"
dst = "/~/"
task_data.add_item(src, dst)

task_doc = tc.submit_transfer(task_data)
task_id = task_doc["task_id"]
print(f"Submitted transfer, task_id={task_id}")
