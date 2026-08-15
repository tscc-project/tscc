interface Item204 { id: number; name: string; }
type Key204 = string | number;
export function make204(id: number, name: string): Item204 {
  const value: Item204 = {id, name};
  return value as Item204;
}
export const item204: Item204 = make204(204, "item-204");
