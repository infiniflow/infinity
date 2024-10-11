const baseUrl = 'http://127.0.0.1:23820/';

export const request = async (
  url: string,
  params: Record<string, any> = {},
  method: string = 'GET'
) => {
  let nextUrl = `${baseUrl}${url}`;
  const options: any = {
    headers: {
      accept: 'application/json'
    },
    cache: 'no-store',
    method
  };

  if (method === 'GET') {
    nextUrl += '?' + new URLSearchParams(params).toString();
  } else {
    options.body = JSON.stringify(params);
  }

  try {
    const res = await fetch(nextUrl, options);
    const ret = await res.json();
    return ret;
  } catch (error) {
    console.warn('request error:', error);
  }
};

export const get = (url: string, params?: Record<string, string>) =>
  request(url, params, 'GET');

export const post = (url: string, params: Record<string, any>) =>
  request(url, params, 'POST');

export const drop = (url: string, params: Record<string, string>) =>
  request(url, params, 'DELETE');
