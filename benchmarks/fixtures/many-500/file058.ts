interface Item58 { id: number; name: string; }
type Key58 = string | number;
export function make58(id: number, name: string): Item58 {
  const value: Item58 = {id, name};
  return value as Item58;
}
export const item58: Item58 = make58(58, "item-58");
