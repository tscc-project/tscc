interface Item314 { id: number; name: string; }
type Key314 = string | number;
export function make314(id: number, name: string): Item314 {
  const value: Item314 = {id, name};
  return value as Item314;
}
export const item314: Item314 = make314(314, "item-314");
