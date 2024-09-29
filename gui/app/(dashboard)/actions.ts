'use server';

import { ApiUrl } from '@/lib/constant/api';
import { CreateOption, DropOption } from '@/lib/constant/common';
import { drop, get, post } from '@/lib/request';

export const listDatabase = async () => {
  try {
    const x = await get(`${ApiUrl.databases}`);
    console.log('🚀 ~ x:', x);
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};

export const createDatabase = async (params: {
  database_name: string;
  create_option: CreateOption;
}) => {
  try {
    const x = await post(`${ApiUrl.databases}/${params.database_name}`, {
      create_option: params.create_option
    });
    console.log('🚀 ~ x:', x);
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};

export const dropDatabase = async (params: {
  database_name: string;
  drop_option: DropOption;
}) => {
  try {
    const x = await drop(`${ApiUrl.databases}/${params.database_name}`, {
      drop_option: params.drop_option
    });
    console.log('🚀 ~ x:', x);
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};

export const showDatabase = async (params: { database_name: string }) => {
  try {
    const x = await get(`${ApiUrl.database}/${params.database_name}`);
    console.log('🚀 ~ x:', x);
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};
