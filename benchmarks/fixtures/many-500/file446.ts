interface Item446 { id: number; name: string; }
type Key446 = string | number;
export function make446(id: number, name: string): Item446 {
  const value: Item446 = {id, name};
  return value as Item446;
}
export const item446: Item446 = make446(446, "item-446");
