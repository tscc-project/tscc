interface Item303 { id: number; name: string; }
type Key303 = string | number;
export function make303(id: number, name: string): Item303 {
  const value: Item303 = {id, name};
  return value as Item303;
}
export const item303: Item303 = make303(303, "item-303");
