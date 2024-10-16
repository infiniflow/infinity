'use server';

import { ApiUrl } from '@/lib/constant/api';
import { CreateOption, DropOption } from '@/lib/constant/common';
import {
  ITableColumns,
  ITableIndex,
  ITableSegment
} from '@/lib/databse-interface';
import { drop, get, post } from '@/lib/request';
import { isResponseListExist } from '@/lib/utils';
import { unstable_rethrow } from 'next/navigation';

export const listDatabase = async () => {
  try {
    const x = await get(`${ApiUrl.databases}`);
    console.log('🚀 ~ x:', x);
    return x;
  } catch (error) {
    unstable_rethrow(error);
    return { databases: [] };
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

//#region table

export const listTable = async (database_name: string) => {
  try {
    const x = await get(
      `${ApiUrl.databases}/${database_name}/${ApiUrl.tables}`
    );
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};

export const createTable = async ({
  database_name,
  table_name,
  create_option,
  fields
}: {
  database_name: string;
  table_name: string;
  fields: Array<{ name: string; type: string; default?: string }>;
  create_option: CreateOption;
}) => {
  try {
    const x = await post(
      `${ApiUrl.databases}/${database_name}/${ApiUrl.tables}/${table_name}`,
      {
        create_option,
        fields
      }
    );
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};

export const dropTable = async ({
  database_name,
  table_name,
  drop_option
}: {
  database_name: string;
  table_name: string;
  drop_option: DropOption;
}) => {
  try {
    const x = await drop(
      `${ApiUrl.databases}/${database_name}/${ApiUrl.tables}/${table_name}`,
      {
        drop_option
      }
    );
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};

export const showTable = async ({
  database_name,
  table_name
}: {
  database_name: string;
  table_name: string;
}) => {
  try {
    const x = await get(
      `${ApiUrl.databases}/${database_name}/${ApiUrl.tables}/${table_name}`
    );
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
  }
};

export const showConfigs = async () => {
  try {
    const x = await get(`${ApiUrl.configs}`);
    return x;
  } catch (error) {
    console.log('🚀 ~ error:', error);
    unstable_rethrow(error);
    return {};
  }
};

export const showVariables = async () => {
  try {
    const x = await get(`${ApiUrl.variables}/global`);
    return x;
  } catch (error) {
    unstable_rethrow(error);
    console.log('🚀 ~ error:', error);
    return {};
  }
};

export const showTableColumns = async ({
  database_name,
  table_name
}: {
  database_name: string;
  table_name: string;
}): Promise<ITableColumns[]> => {
  try {
    const x = await get(
      `${ApiUrl.databases}/${database_name}/${ApiUrl.tables}/${table_name}/${ApiUrl.columns}`
    );
    if (isResponseListExist(x, 'columns')) {
      return x.columns;
    }
    return [];
  } catch (error: unknown) {
    console.log('🚀 ~ error:', error);
    return [];
  }
};

export const showTableIndexes = async ({
  database_name,
  table_name
}: {
  database_name: string;
  table_name: string;
}): Promise<ITableIndex[]> => {
  try {
    const x = await get(
      `${ApiUrl.databases}/${database_name}/${ApiUrl.tables}/${table_name}/${ApiUrl.indexes}`
    );
    if (isResponseListExist(x, 'indexes')) {
      return x.indexes;
    }
    return [];
  } catch (error: unknown) {
    console.log('🚀 ~ error:', error);
    return [];
  }
};

export const showTableSegments = async ({
  database_name,
  table_name
}: {
  database_name: string;
  table_name: string;
}): Promise<ITableSegment[]> => {
  try {
    const x = await get(
      `${ApiUrl.databases}/${database_name}/${ApiUrl.tables}/${table_name}/${ApiUrl.segments}`
    );
    if (isResponseListExist(x, 'segments')) {
      return x?.segments ?? [];
    }
    return [];
  } catch (error: unknown) {
    console.log('🚀 ~ error:', error);
    return [];
  }
};
//#endregion
