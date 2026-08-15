interface Item211 { id: number; name: string; }
type Key211 = string | number;
export function make211(id: number, name: string): Item211 {
  const value: Item211 = {id, name};
  return value as Item211;
}
export const item211: Item211 = make211(211, "item-211");
