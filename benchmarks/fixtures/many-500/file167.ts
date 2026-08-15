interface Item167 { id: number; name: string; }
type Key167 = string | number;
export function make167(id: number, name: string): Item167 {
  const value: Item167 = {id, name};
  return value as Item167;
}
export const item167: Item167 = make167(167, "item-167");
