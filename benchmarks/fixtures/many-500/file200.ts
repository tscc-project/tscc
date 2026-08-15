interface Item200 { id: number; name: string; }
type Key200 = string | number;
export function make200(id: number, name: string): Item200 {
  const value: Item200 = {id, name};
  return value as Item200;
}
export const item200: Item200 = make200(200, "item-200");
