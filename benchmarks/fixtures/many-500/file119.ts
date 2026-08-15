interface Item119 { id: number; name: string; }
type Key119 = string | number;
export function make119(id: number, name: string): Item119 {
  const value: Item119 = {id, name};
  return value as Item119;
}
export const item119: Item119 = make119(119, "item-119");
