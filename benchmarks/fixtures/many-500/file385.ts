interface Item385 { id: number; name: string; }
type Key385 = string | number;
export function make385(id: number, name: string): Item385 {
  const value: Item385 = {id, name};
  return value as Item385;
}
export const item385: Item385 = make385(385, "item-385");
