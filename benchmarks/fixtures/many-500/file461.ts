interface Item461 { id: number; name: string; }
type Key461 = string | number;
export function make461(id: number, name: string): Item461 {
  const value: Item461 = {id, name};
  return value as Item461;
}
export const item461: Item461 = make461(461, "item-461");
