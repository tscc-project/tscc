interface Item455 { id: number; name: string; }
type Key455 = string | number;
export function make455(id: number, name: string): Item455 {
  const value: Item455 = {id, name};
  return value as Item455;
}
export const item455: Item455 = make455(455, "item-455");
