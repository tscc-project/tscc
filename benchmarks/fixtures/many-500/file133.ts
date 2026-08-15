interface Item133 { id: number; name: string; }
type Key133 = string | number;
export function make133(id: number, name: string): Item133 {
  const value: Item133 = {id, name};
  return value as Item133;
}
export const item133: Item133 = make133(133, "item-133");
