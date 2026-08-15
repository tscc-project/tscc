interface Item247 { id: number; name: string; }
type Key247 = string | number;
export function make247(id: number, name: string): Item247 {
  const value: Item247 = {id, name};
  return value as Item247;
}
export const item247: Item247 = make247(247, "item-247");
