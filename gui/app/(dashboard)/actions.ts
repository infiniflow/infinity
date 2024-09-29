'use server';

import { ApiUrl } from '@/lib/constant/api';
import { CreateOption } from '@/lib/constant/common';
import { get, post } from '@/lib/request';

export async function deleteProduct(formData: FormData) {
  // let id = Number(formData.get('id'));
  // await deleteProductById(id);
  // revalidatePath('/');
}

export async function createDatabaseAction(formData: FormData) {
  // let id = Number(formData.get('id'));
  // await deleteProductById(id);
  // revalidatePath('/');
}

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

export const listDatabase = async () => {
  try {
    const x = await get(`${ApiUrl.databases}`);
    console.log('🚀 ~ x:', x);
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};
