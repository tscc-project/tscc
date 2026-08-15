interface Item265 { id: number; name: string; }
type Key265 = string | number;
export function make265(id: number, name: string): Item265 {
  const value: Item265 = {id, name};
  return value as Item265;
}
export const item265: Item265 = make265(265, "item-265");
