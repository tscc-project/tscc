interface Item215 { id: number; name: string; }
type Key215 = string | number;
export function make215(id: number, name: string): Item215 {
  const value: Item215 = {id, name};
  return value as Item215;
}
export const item215: Item215 = make215(215, "item-215");
