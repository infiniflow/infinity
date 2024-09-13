import { ApiUrl } from '../constant/api';
import { post } from '../request';

export const createDatabase = async (databaseName: string) => {
  return post(`${ApiUrl.databases}/${databaseName}`, { create_option: 'post' });
};
