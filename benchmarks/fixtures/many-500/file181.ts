interface Item181 { id: number; name: string; }
type Key181 = string | number;
export function make181(id: number, name: string): Item181 {
  const value: Item181 = {id, name};
  return value as Item181;
}
export const item181: Item181 = make181(181, "item-181");
