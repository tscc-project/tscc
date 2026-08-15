interface Item240 { id: number; name: string; }
type Key240 = string | number;
export function make240(id: number, name: string): Item240 {
  const value: Item240 = {id, name};
  return value as Item240;
}
export const item240: Item240 = make240(240, "item-240");
