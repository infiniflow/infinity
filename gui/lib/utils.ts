import { type ClassValue, clsx } from 'clsx';
import { twMerge } from 'tailwind-merge';

const baseUrl = 'http://127.0.0.1:23820/';

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs));
}

export const request = async (url: string, params?: any) => {
  const nextUrl = `${baseUrl}${url}`;
  try {
    const res = await fetch(nextUrl, {
      headers: {
        accept: 'application/json'
      },
      cache: 'no-store',
      ...params
    });
    const ret = await res.json();
    return ret;
  } catch (error) {
    console.warn('request error:', error);
  }
};
