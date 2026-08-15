interface Item128 { id: number; name: string; }
type Key128 = string | number;
export function make128(id: number, name: string): Item128 {
  const value: Item128 = {id, name};
  return value as Item128;
}
export const item128: Item128 = make128(128, "item-128");
