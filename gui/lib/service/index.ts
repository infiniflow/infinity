import { ApiUrl } from '../constant/api';
import { CreateOption } from '../constant/common';
import { post } from '../request';

export const createDatabase = async (
  databaseName: string,
  createOption: CreateOption
) => {
  return post(`${ApiUrl.databases}/${databaseName}`, {
    create_option: createOption
  });
};
