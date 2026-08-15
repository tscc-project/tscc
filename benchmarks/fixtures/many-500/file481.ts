interface Item481 { id: number; name: string; }
type Key481 = string | number;
export function make481(id: number, name: string): Item481 {
  const value: Item481 = {id, name};
  return value as Item481;
}
export const item481: Item481 = make481(481, "item-481");
