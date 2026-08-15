interface Item235 { id: number; name: string; }
type Key235 = string | number;
export function make235(id: number, name: string): Item235 {
  const value: Item235 = {id, name};
  return value as Item235;
}
export const item235: Item235 = make235(235, "item-235");
